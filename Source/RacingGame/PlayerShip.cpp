// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShip.h"
//#include "GameFramework/PlayerInput.h"
//#include "Components/InputComponent.h"
//#include "Engine/World.h"
//#include "Camera/CameraActor.h"
//#include "Engine/Engine.h"
//#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
//#include "Particles/ParticleSystemComponent.h"
#include "GenericPlatform/GenericPlatformMisc.h"


// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RtRpl = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	SetRootComponent(RtRpl);
	//RtRpl->SetSimulatePhysics(true);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerShip::OnOverlapBegin);

	DashTimer = 2.f;
	MaxSpeedBoost = 4.f;
	SpeedBoost = 1.f;
	InitialArmLength = 1000.f;
	MaxHealth = 300.f;
	Health = MaxHealth;
	InitialMaterial = BaseMesh->GetMaterial(0);
	bIsDashing = false;
	bIsJumping = false;
	EnemyCooldownTime = 1.f;
	IgnoreInput = false;
	GameWon = false;
	LocalMove = FVector::ZeroVector;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = InitialArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 25.f; // Lower = More delay
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));

	UArrowComponent* Thrust1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust1"));
	UArrowComponent* Thrust2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust2"));
	UArrowComponent* Thrust3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust3"));
	UArrowComponent* Thrust4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust4"));

	Thrust1->SetRelativeLocationAndRotation(FVector(354.f, -432.f, -23.f), FRotator(-95.f, 0.f, 0.f));
	Thrust2->SetRelativeLocationAndRotation(FVector(354.f, 432.f, -23.f), FRotator(-95.f, 0.f, 0.f));
	Thrust3->SetRelativeLocationAndRotation(FVector(-66.f, -416.f, 82.f), FRotator(-95.f, 0.f, 0.f));
	Thrust4->SetRelativeLocationAndRotation(FVector(-66.f, 416.f, 82.f), FRotator(-95.f, 0.f, 0.f));

	ThrustLocations.Reserve(4);
	ThrustLocations.Add(Thrust1);
	ThrustLocations.Add(Thrust2);
	ThrustLocations.Add(Thrust3);
	ThrustLocations.Add(Thrust4);

	for (int i{}; i < ThrustLocations.Num(); i++) { ThrustLocations[i]->SetupAttachment(GetRootComponent()); }

	ThrustFX1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX1"));
	ThrustFX2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX2"));
	ThrustFX3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX3"));
	ThrustFX4 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX4"));
	
	/**
	* The reason I am doing it like this instead of using a TArray is that UE crashes when using
	* UParticleSystemComponent->Activate() since the UParticleSystemComponent is NULL.
	* Don't know if this is a bug, but this works at least
	*/
	ThrustFX1->SetupAttachment(ThrustLocations[0]);
	ThrustFX2->SetupAttachment(ThrustLocations[1]);
	ThrustFX3->SetupAttachment(ThrustLocations[2]);
	ThrustFX4->SetupAttachment(ThrustLocations[3]);

	ThrustFX1->bAutoActivate = false;
	ThrustFX2->bAutoActivate = false;
	ThrustFX3->bAutoActivate = false;
	ThrustFX4->bAutoActivate = false;

	DeathFXComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DeathFX"));
	DeathFXComponent->SetupAttachment(GetRootComponent());
	DeathFXComponent->bAutoActivate = false;
	DeathFXComponent->SetRelativeScale3D(FVector(7.f, 7.f, 7.f));
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	/** Apply effects to all subobjects, so you don't have to assign 4 individual values in blueprints */
	if (ThrustFX)
	{
		ThrustFX1->SetTemplate(ThrustFX);
		ThrustFX2->SetTemplate(ThrustFX);
		ThrustFX3->SetTemplate(ThrustFX);
		ThrustFX4->SetTemplate(ThrustFX);
	}

	if (DeathFX)
	{
		DeathFXComponent->SetTemplate(DeathFX);
	}

	//Force.Z = RtRpl->GetMass() * 100 * 9.80665;
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShootTimer += DeltaTime;

	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));

	FVector CurLoc = GetActorLocation();

	CurLoc.Z = FMath::FInterpTo(CurLoc.Z, TargetZ, DeltaTime, 3.f);
	SetActorLocation(CurLoc);

	AddActorLocalOffset(LocalMove * DeltaTime * 110.f, true);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewUpVector, DeltaTime, 2.f));
	FRotator New2Rot = GetActorRotation();
	New2Rot.Yaw = NextYawPosition;
	SetActorRotation(New2Rot);

	/** Springarm rotation */
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(SpringArmRotation, GetActorRotation(), DeltaTime, 25.f);
	NewRot.Pitch = SpringArmRotation.Pitch;
	NewRot.Roll = 0;
	SpringArm->SetWorldRotation(NewRot);
	

	FVector A, B, C, AB, AC;

	/** Raycasting advanced */
	for (int i{}; i < 4; i++)
	{
		FHitResult OutHit;
		FVector Start = ThrustLocations[i]->GetComponentLocation();
		FVector End = Start;
		End.Z -= 5000.f;
		FCollisionQueryParams CollisionParams;

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 5.f);
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), OutHit.Location, 50.f, 12, FColor::Blue);
			}
		}

		

		switch (i)
		{
		case 0:
			break;
		case 1:
			B = OutHit.Location;
			break;
		case 2:
			C = OutHit.Location;
			break;
		case 3:
			A = OutHit.Location;
			break;
		default:
			break;
		}
	}

	AB = B - A;
	AC = C - A;
	FVector NUV = FVector::CrossProduct(AC, AB);

	NewUpVector = UKismetMathLibrary::MakeRotFromZX(NUV, GetActorForwardVector());

	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + NUV * 15.f, FColor::Red, true);

	/** Raycasting */
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= 10000.f;
	FCollisionQueryParams CollisionParams;
	bool BelowNormal = false;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2, 0, 2);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("\nStart Z: %f\nImpact Z: %f\nDiff: %f"), Start.Z, OutHit.ImpactPoint.Z, Start.Z - OutHit.ImpactPoint.Z)

			//ForceChange = 3.f;
			TargetZ = OutHit.ImpactPoint.Z + 1000.f;
			BelowNormal = true;
		}
	}
	if (!BelowNormal)
	{
		//ForceChange = 0.5f;
		//LocalMove.Z = -1.f;
		TargetZ = 0.f;
	}
	
}


void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//InitializeDefaultPawnInputBinding();

	PlayerInputComponent->BindAxis("Forward", this, &APlayerShip::Pitch);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShip::Roll);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShip::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShip::Yaw);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShip::Jump);
	PlayerInputComponent->BindAction("Esc", EInputEvent::IE_Pressed, this, &APlayerShip::EscPressed);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShip::Pitch(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -25.0f : 25.0f : 0.f;

	// Interpolate rotation towards target
	//NextPitchPosition = FMath::FInterpTo(GetActorRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);
	//NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	float TargetXSpeed = bPitchHasInput ? (Value * 40.f * SpeedBoost) : 0.f;
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	Force.X = bPitchHasInput ? Value : 0.f;
}


void APlayerShip::Roll(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bRollHasInput = !(Value == 0);
	// If there is input, set rotation target to -30/30 based on input value, else set target to 0
	float TargetRoll = bRollHasInput ? Value > 0 ? 30.0f : -30.0f : 0.f;
	// Interpolate rotation towards target
	//NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 6.f);

	float TargetYSpeed = bRollHasInput ? (Value * 30.f) : 0.f;
	LocalMove.Y = FMath::FInterpTo(LocalMove.Y, TargetYSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	Force.Y = bRollHasInput ? Value : 0.f;
}


void APlayerShip::Yaw(float Value)
{
	if (IgnoreInput) { return; }
	NextYawPosition = GetActorRotation().Yaw + Value * GetWorld()->GetDeltaSeconds() * 50.f;
}


void APlayerShip::CameraPitch(float Value)
{
	if (!Value || IgnoreInput) { return; }
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	float TargetPitch = FMath::Clamp(CurrentRot.Pitch + Value * 15.f, -80.f, 0.f);
	CurrentRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);

	SpringArm->SetRelativeRotation(CurrentRot);
}


void APlayerShip::Dash() 
{
	if (IgnoreInput) { return; }

	if (bIsDashing) 
	{
		return;
	}
	
	SpeedBoost = MaxSpeedBoost;
	bIsDashing = true;

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::ResetDash, DashTimer, false);

	if (ThrustFX)
	{
		ThrustFX1->ResetParticles();
		ThrustFX2->ResetParticles();
		ThrustFX3->ResetParticles();
		ThrustFX4->ResetParticles();

		ThrustFX1->Activate();
		ThrustFX2->Activate();
		ThrustFX3->Activate();
		ThrustFX4->Activate();
	}
}


void APlayerShip::ResetDash()
{
	SpeedBoost = 1.f;
	bIsDashing = false;
}


void APlayerShip::Jump()
{
	if (IgnoreInput) { return; }

	if (bIsJumping)
	{
		return;
	}

	//bIsJumping = true;
	RtRpl->AddImpulse(FVector(0.f, 0.f, 10000000.f));
	bIsJumping = true;

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::JumpEnd, 4.f, false);

	if (ThrustFX)
	{
		ThrustFX1->ResetParticles();
		ThrustFX2->ResetParticles();
		ThrustFX3->ResetParticles();
		ThrustFX4->ResetParticles();

		ThrustFX1->Activate();
		ThrustFX2->Activate();
		ThrustFX3->Activate();
		ThrustFX4->Activate();
	}
}


void APlayerShip::JumpEnd()
{
	bIsJumping = false;
}


void APlayerShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent || IgnoreInput) { return; }

	
}


FVector APlayerShip::GetLoc()
{
	return GetActorLocation();
}


void APlayerShip::EscPressed()
{
	FGenericPlatformMisc::RequestExit(false);
}


