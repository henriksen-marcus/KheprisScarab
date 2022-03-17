// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipPhysics.h"

// Sets default values
APlayerShipPhysics::APlayerShipPhysics()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RtRpl = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	SetRootComponent(RtRpl);
	RtRpl->SetSimulatePhysics(true);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BaseMesh->SetupAttachment(GetRootComponent());
	//BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);

	DashTimer = 2.f;
	SpeedBoost = 1.f;
	InitialArmLength = 1000.f;
	bIsDashing = false;
	bIsJumping = false;
	IgnoreInput = false;

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
}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	/** Apply effects to all subobjects, so you don't have to assign 4 individual values in blueprints */

	//Force.Z = RtRpl->GetMass() * 100 * 9.80665;
}


void APlayerShipPhysics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	/** Ship movement - Changed to physics based */ 
	FVector CombinedVectors = FVector::ZeroVector;
	CombinedVectors += Force.X * GetActorForwardVector();
	CombinedVectors += Force.Y * GetActorRightVector();
	CombinedVectors *= 20000000 * SpeedBoost;
	if (JumpCurve)
	{
		CombinedVectors.Z = ForceChange * Force.Z;
	}
	

	RtRpl->AddForce(CombinedVectors);
	RtRpl->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	FVector Resistance = FVector::ZeroVector;
	Resistance = RtRpl->GetPhysicsLinearVelocity() * -1000.f;
	Resistance.Z = 0;
	RtRpl->AddForce(Resistance);

	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));

	FVector CurLoc = GetActorLocation();

	//float temp2 = FMath::Abs(0.049f * CurLoc.Z - TargetZ + 1.f);
	//float CurveStr = FMath::Clamp(temp2, 1.f, 50.f);
	//float CurveStr = FMath::Abs(1000 / CurLoc.Z - TargetZ);
	//UE_LOG(LogTemp, Warning, TEXT("CurveStr: %f\nHeight: %f"), CurveStr, CurLoc.Z - TargetZ)


	SetActorRotation(FRotator(0.f, NextYawPosition, 0.f));

	/** Springarm rotation */
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(SpringArmRotation, GetActorRotation(), DeltaTime, 25.f);
	NewRot.Pitch = SpringArmRotation.Pitch;
	NewRot.Roll = 0;
	SpringArm->SetWorldRotation(NewRot);

	/** Raycasting */
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= 10000.f;
	FCollisionQueryParams CollisionParams;
	bool BelowNormal = false;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2, 0, 2);

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


void APlayerShipPhysics::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//InitializeDefaultPawnInputBinding();

	PlayerInputComponent->BindAxis("Forward", this, &APlayerShipPhysics::Pitch);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShipPhysics::Roll);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShipPhysics::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShipPhysics::Yaw);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Jump);
	PlayerInputComponent->BindAction("Esc", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::EscPressed);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShipPhysics::Pitch(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -25.0f : 25.0f : 0.f;

	// Interpolate rotation towards target
	//NextPitchPosition = FMath::FInterpTo(GetActorRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	Force.X = bPitchHasInput ? Value : 0.f;
}


void APlayerShipPhysics::Roll(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bRollHasInput = !(Value == 0);
	// If there is input, set rotation target to -30/30 based on input value, else set target to 0
	float TargetRoll = bRollHasInput ? Value > 0 ? 30.0f : -30.0f : 0.f;
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 6.f);

	Force.Y = bRollHasInput ? Value : 0.f;
}


void APlayerShipPhysics::Yaw(float Value)
{
	if (IgnoreInput) { return; }
	NextYawPosition = GetActorRotation().Yaw + Value * GetWorld()->GetDeltaSeconds() * 50.f;
}


void APlayerShipPhysics::CameraPitch(float Value)
{
	if (!Value || IgnoreInput) { return; }
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	float TargetPitch = FMath::Clamp(CurrentRot.Pitch + Value * 15.f, -50.f, 0.f);
	CurrentRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);

	SpringArm->SetRelativeRotation(CurrentRot);
}


void APlayerShipPhysics::Dash()
{
	if (IgnoreInput) { return; }

	if (bIsDashing)
	{
		return;
	}

	bIsDashing = true;

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShipPhysics::ResetDash, DashTimer, false);
}


void APlayerShipPhysics::ResetDash()
{
	SpeedBoost = 1.f;
	bIsDashing = false;
}


void APlayerShipPhysics::Jump()
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
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShipPhysics::JumpEnd, 4.f, false);

}


void APlayerShipPhysics::JumpEnd()
{
	bIsJumping = false;
}


void APlayerShipPhysics::EscPressed()
{
	FGenericPlatformMisc::RequestExit(false);
}

