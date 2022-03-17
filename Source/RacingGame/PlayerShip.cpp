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

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerShip::OnOverlapBegin);

	DashTimer = 2.f;
	MaxSpeedBoost = 2.f;
	SpeedBoost = 1.f;
	InitialArmLength = 1000.f;
	bIsDashing = false;
	bIsJumping = false;
	LocalMove = FVector::ZeroVector;
	SpeedMultiplier = 1.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	//SpringArm->SetUsingAbsoluteRotation(false);
	//SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = InitialArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f; // Lower = More delay
	SpringArm->bDoCollisionTest = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));

	UArrowComponent* Thrust1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust1"));
	UArrowComponent* Thrust2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust2"));
	UArrowComponent* Thrust3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust3"));
	//UArrowComponent* Thrust4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust4"));

	Thrust1->SetRelativeLocationAndRotation(FVector(500.f, -300.f, 0.f), FRotator(-90.f, 0.f, 0.f));
	Thrust2->SetRelativeLocationAndRotation(FVector(500.f, 300.f, 0.f), FRotator(-90.f, 0.f, 0.f));
	Thrust3->SetRelativeLocationAndRotation(FVector(-500.f, 0.f, 0.f), FRotator(-90.f, 0.f, 0.f));
	//Thrust4->SetRelativeLocationAndRotation(FVector(-66.f, 416.f, 82.f), FRotator(-95.f, 0.f, 0.f));

	ThrustLocations.Reserve(4);
	ThrustLocations.Add(Thrust1);
	ThrustLocations.Add(Thrust2);
	ThrustLocations.Add(Thrust3);
	//ThrustLocations.Add(Thrust4);

	for (int i{}; i < ThrustLocations.Num(); i++) { ThrustLocations[i]->SetupAttachment(GetRootComponent());}
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Z Movement
	FVector CurLoc = GetActorLocation();
	CurLoc.Z = FMath::FInterpTo(CurLoc.Z, TargetZ, DeltaTime, 3.f);
	SetActorLocation(CurLoc);

	// X and Y Movement
	AddActorLocalOffset(LocalMove * DeltaTime * 110.f * SpeedBoost * SpeedMultiplier, true);

	// Root Rotation
	FRotator TargetRotation = GetSurfaceNormal();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 2.f));
	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));

	// Cosmetic mesh rotation
	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));

	/** Springarm rotation */
	CameraMove.Pitch = FMath::ClampAngle(CameraMove.Pitch, -70.f, -10.f);
	CameraMove.Yaw = FMath::ClampAngle(CameraMove.Yaw, -70.f, 70.f);
	
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(SpringArmRotation, CameraMove, DeltaTime, 10.f);
	//NewRot.Pitch = SpringArmRotation.Pitch;
	NewRot.Roll = 0;
	SpringArm->SetRelativeRotation(NewRot);
	//SpringArm->SetWorldRotation(GetActorRotation());
	
	
	/** Raycasting - Keeping the ship afloat */
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= 10000.f;
	FCollisionQueryParams CollisionParams;
	bool BelowNormal = false;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 8.f);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit)
		{
			TargetZ = OutHit.ImpactPoint.Z + 1000.f;
			BelowNormal = true;
		}
	}
	if (!BelowNormal)
	{
		TargetZ = Start.Z - 2000.f;
	}
	
}


void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Forward", this, &APlayerShip::Forward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShip::Turn);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShip::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShip::CameraYaw);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShip::Jump);
	PlayerInputComponent->BindAction("Esc", EInputEvent::IE_Pressed, this, &APlayerShip::EscPressed);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShip::Forward(const float Value)
{
	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	const float TargetXSpeed = bPitchHasInput ? (Value * 40.f * SpeedBoost) : 0.f;
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	
}


void APlayerShip::Turn(const float Value)
{
	// Determine if there is input
	bRollHasInput = !(Value == 0);

	/** Roll */
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	/** Yaw */
	YawMove = FMath::FInterpTo(YawMove,  0.8f * Value, GetWorld()->GetDeltaSeconds(), 8.f);
	//NextYawPosition = FMath::FInterpTo(GetActorRotation().Yaw, GetActorRotation().Yaw + Value * 40.f, GetWorld()->GetDeltaSeconds(), 0.5f);
	
	//const float TargetYSpeed = bRollHasInput ? (Value * 30.f) : 0.f;
	//LocalMove.Y = FMath::FInterpTo(LocalMove.Y, TargetYSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void APlayerShip::CameraYaw(const float Value)
{
	if (!Value) { return; }
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	//CurrentRot.Yaw = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);
	CameraMove.Yaw = SpringArm->GetRelativeRotation().Yaw + Value * 15.f;
	CurrentRot.Yaw = FMath::Clamp(CurrentRot.Yaw + Value, -40.f, 40.f);
	//SpringArm->SetRelativeRotation(CurrentRot);
	
	//NextYawPosition = GetActorRotation().Yaw + Value * GetWorld()->GetDeltaSeconds() * 50.f;
}


void APlayerShip::CameraPitch(const float Value)
{
	if (!Value) { return; }
	CameraMove.Pitch = SpringArm->GetRelativeRotation().Pitch + Value * 15.f;
	//CurrentRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);

	//SpringArm->SetRelativeRotation(CurrentRot);
}


void APlayerShip::Dash() 
{
	if (bIsDashing) 
	{
		return;
	}
	
	SpeedBoost = MaxSpeedBoost;
	bIsDashing = true;

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::ResetDash, DashTimer, false);
}


void APlayerShip::ResetDash()
{
	SpeedBoost = 1.f;
	bIsDashing = false;
}


void APlayerShip::Jump()
{
	return; // Jumping is not currently supported

	if (bIsJumping)
	{
		return;
	}
	bIsJumping = true;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::JumpEnd, 4.f, false);
}


void APlayerShip::JumpEnd()
{
	bIsJumping = false;
}


void APlayerShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }

}

void APlayerShip::EscPressed()
{
	FGenericPlatformMisc::RequestExit(false);
}

FRotator APlayerShip::GetSurfaceNormal()
{
	/** Raycasting - making the ship match the ground's rotation */

	// We need three points and two vectors to determine the cross product and get the new up vector
	FVector A, B, C, VecAb, VecAc;
	
	for (int i{}; i < 3; i++)
	{
		FHitResult OutHit;
		FVector Start = ThrustLocations[i]->GetComponentLocation();
		FVector End = Start;
		End.Z -= 2000.f;
		FCollisionQueryParams CollisionParams;

		// Raycast visualization, red line
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 12.f);

		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			// If raycast hit something
			if (OutHit.bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), OutHit.Location, 60.f, 16, FColor::Blue);

				switch (i)
				{
				case 0:
					A = OutHit.Location;
					break;
				case 1:
					B = OutHit.Location;
					break;
				case 2:
					C = OutHit.Location;
					break;
				default:
					break;
				}
			}
		}
	}
	
	// Check if we are getting collision on all points before doing the math
	if (A != FVector::ZeroVector && B != FVector::ZeroVector && C != FVector::ZeroVector)
	{
		VecAb = C - A;
		VecAc = B - A;
		
		FVector NewUpVector = FVector::CrossProduct(VecAc, VecAb);
		NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());

		// Clamp angles so that  the ship cannot flip
		NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
		NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
		
		// Upwards line
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 1000, FColor::Green, false, -1.f, 0, 12.f);
	} else
	{
		NewRotation = FRotator::ZeroRotator;
	}
	return NewRotation;
}


