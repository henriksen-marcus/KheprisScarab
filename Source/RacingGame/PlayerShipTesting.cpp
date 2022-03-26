// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipTesting.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/WorldSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"


// Sets default values
APlayerShipTesting::APlayerShipTesting()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RtRpl = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	SetRootComponent(RtRpl);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BaseMesh->SetupAttachment(GetRootComponent());
	
	bIsDashing = false;
	bIsJumping = false;
	LocalMove = FVector::ZeroVector;
	SpeedMultiplier = 1.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	//SpringArm->SetUsingAbsoluteRotation(false);
	//SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = TargetSpringArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 30.f; // Lower = More delay
	SpringArm->bDoCollisionTest = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(12.f, 0.f, 0.f));

	 Thrust1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust1"));
	 Thrust2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust2"));
	 Thrust3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust3"));
	 Thrust4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust4"));

	Thrust1->SetupAttachment(GetRootComponent());
	Thrust2->SetupAttachment(GetRootComponent());
	Thrust3->SetupAttachment(GetRootComponent());
	Thrust4->SetupAttachment(GetRootComponent());
	
	//for (int i{}; i < ThrustLocations.Num(); i++) { ThrustLocations[i]->SetupAttachment(GetRootComponent());}
	
	Thrust1->SetRelativeLocationAndRotation(FVector(200.f, -200.f, 0.f), FRotator(-60.f, -45.f, 0.f));
	Thrust2->SetRelativeLocationAndRotation(FVector(200.f, 200.f, 0.f), FRotator(-60.f, 45.f, 0.f));
	Thrust3->SetRelativeLocationAndRotation(FVector(-200.f, -200.f, 0.f), FRotator(-120.f, 45.f, 0.f));
	Thrust4->SetRelativeLocationAndRotation(FVector(-200.f, 200.f, 0.f), FRotator(-120.f, -45.f, 0.f));
}


void APlayerShipTesting::BeginPlay()
{
	Super::BeginPlay();
}


void APlayerShipTesting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator TargetRot  = GetSurfaceNormal();
	
	// X and Y Movement
	FRotator RotAmount = UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), GetActorUpVector());
	FVector Res = RotAmount.RotateVector(LocalMove)*50;
	//FVector Result = GetActorRotation().RotateVector(LocalMove) * 50;
	//SetActorLocation(TargetLocation + Res);
	SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation + Res, DeltaTime, 10.f)); 
	//UE_LOG(LogTemp, Warning, TEXT("TargetLoc = %s"), *TargetLocation.ToString())
	//AddActorLocalOffset(LocalMove);
	//AddActorLocalOffset(LocalMove * DeltaTime * 110.f * SpeedBoost * SpeedMultiplier, true);

	// Root Rotation
	//AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	//TargetRot.Pitch = FMath::FInterpTo(GetActorRotation().Pitch, TargetRot.Pitch, DeltaTime, 10.f);
	//TargetRot.Roll = FMath::FInterpTo(GetActorRotation().Roll, TargetRot.Roll, DeltaTime, 10.f);
	//TargetRot.Yaw = FMath::FInterpTo(GetActorRotation().Yaw, TargetRot.Yaw, DeltaTime, 10.f);
	//TargetRot.Yaw = GetActorRotation().Yaw;

	SetActorRotation(TargetRot);
	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	
	//RtRpl->SetWorldRotation(abcdefg);
	//RtRpl->AddLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	//SetActorRotation(FMath::RInterpTo(GetActorRotation(), DeltaRot, DeltaTime, 10.f));
	
	
	/** Springarm rotation */
	FRotator NewRot = SpringArm->GetRelativeRotation();
	NewRot.Pitch = FMath::InterpEaseInOut(NewRot.Pitch, SpringArmRotTarget.Pitch, DeltaTime, 0.6f);
	NewRot.Yaw = FMath::InterpEaseInOut(NewRot.Yaw, SpringArmRotTarget.Yaw, DeltaTime, 0.6f);
	//NewRot.Roll = 0;
	SpringArm->SetRelativeRotation(NewRot);

	/** Camera effects */
	Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetCameraFOV, DeltaTime, 10.f));
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetSpringArmLength, DeltaTime, 10.f);
	
	CameraCenteringTimer += DeltaTime;
}


void APlayerShipTesting::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Forward", this, &APlayerShipTesting::Forward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShipTesting::Turn);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShipTesting::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShipTesting::CameraYaw);
	
	PlayerInputComponent->BindAction("Esc", EInputEvent::IE_Pressed, this, &APlayerShipTesting::EscPressed);

	PlayerInputComponent->BindAction("ScrollUp", EInputEvent::IE_Pressed, this, &APlayerShipTesting::CameraZoomIn);
	PlayerInputComponent->BindAction("ScrollDown", EInputEvent::IE_Pressed, this, &APlayerShipTesting::CameraZoomOut);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShipTesting::Forward(const float Value)
{
	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	const float TargetXSpeed = bPitchHasInput ? (Value * 40.f) : 0.f;
	float InterpSpeed = bPitchHasInput ? 2.f : 0.5f;
	InterpSpeed = Value < 0.f ? InterpSpeed / 4.f : InterpSpeed;
	
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}
void APlayerShipTesting::Turn(const float Value)
{
	// Determine if there is input
	bRollHasInput = !(Value == 0);

	// Roll
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	// Yaw
	YawMove = FMath::FInterpTo(YawMove,  Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);
}
void APlayerShipTesting::CameraYaw(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;
	
	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Yaw = bShouldReset ? 0.f : FMath::Clamp(SpringArm->GetRelativeRotation().Yaw + Value * 5.f, -80.f, 80.f);
}
void APlayerShipTesting::CameraPitch(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;

	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Pitch = bShouldReset ? -20.f : FMath::Clamp(SpringArm->GetRelativeRotation().Pitch + Value * 10.f, -80.f, 0.f);
}
void APlayerShipTesting::CameraZoomIn()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength - 200.f, 1000.f, 3500.f);
}
void APlayerShipTesting::CameraZoomOut()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength + 200.f, 1000.f, 3500.f);
}
void APlayerShipTesting::EscPressed()
{
	FGenericPlatformMisc::RequestExit(false);
}


FRotator APlayerShipTesting::GetSurfaceNormal()
{
	const float RayCastLength = 2000.f;
	
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);

	int32 Counter{};
	
	for (int i{}; i < 4; i++)
	{
		FVector Start, End;
		
		switch (i)
		{
		case 0:
			Start = Thrust1->GetComponentLocation();
			End = Start + Thrust1->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 1:
			Start = Thrust2->GetComponentLocation();
			End = Start + Thrust2->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 2:
			Start = Thrust3->GetComponentLocation();
			End = Start + Thrust3->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 3:
			Start = Thrust4->GetComponentLocation();
			End = Start + Thrust4->GetComponentRotation().Vector() * RayCastLength;
			break;
		default:
			Start = FVector::ZeroVector;
		}
		
		FCollisionQueryParams CollisionParams;

		DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.f, 0, 12.f);
		
		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			// If raycast hit something
			if (HitPoints[i].bBlockingHit)
			{
				// Raycast visuals
				DrawDebugSphere(GetWorld(), HitPoints[i].Location, 60.f, 16, FColor::Blue);
			}
		}
		else
		{
			End.Z += 500;
			HitPoints[i].Location = End;
			++Counter;
		}
	}

	// If we are fully in-air, start to pitch downwards
	if (Counter == 4)
	{
		UE_LOG(LogTemp,Warning,TEXT("In-air"))
		FRotator TempRot = GetActorRotation();
		TempRot.Pitch = FMath::FInterpTo(TempRot.Pitch, -30.f, GetWorld()->GetDeltaSeconds(), 20.f);
		TempRot.Roll = 0.f;
		return TempRot;
	}

	

	// A -> B Vector
	const FVector A_B = HitPoints[1].Location - HitPoints[0].Location;

	// A -> C Vector
	const FVector A_C = HitPoints[2].Location - HitPoints[0].Location;

	// D -> B Vector
	const FVector D_B = HitPoints[1].Location - HitPoints[3].Location;

	// D -> C Vector
	const FVector D_C = HitPoints[2].Location - HitPoints[3].Location;

	const FVector CrossA = FVector::CrossProduct(A_B, A_C);
	const FVector CrossD = FVector::CrossProduct(D_C, D_B);
	FVector NewUpVector = (CrossA + CrossD);
	NewUpVector = HitPoints[0].Normal;

	// Inserted code
	FVector TempVec = HitPoints[0].Location + HitPoints[1].Location + HitPoints[2].Location + HitPoints[3].Location;
	TempVec /= 4;
	TempVec += NewUpVector.GetSafeNormal() * TargetHeight;
	TargetLocation = TempVec;
	DrawDebugSphere(GetWorld(), TargetLocation, 100.f, 16, FColor::Emerald);
	// End inserted code


	//FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());
	FRotator NewRotation = UKismetMathLibrary::MakeRotationFromAxes(-A_C.GetSafeNormal(), A_B,NewUpVector.GetSafeNormal());
	//FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());
	
	// Mass debugging
	DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[0].Location + CrossA.GetSafeNormal() * 1000, FColor::Red, false, -1.f, 0, 12.f);
	DrawDebugLine(GetWorld(), HitPoints[3].Location, HitPoints[3].Location + CrossD.GetSafeNormal() * 1000, FColor::Blue, false, -1.f, 0, 12.f);
	
	DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[1].Location, FColor::White, false, -1.f, 0, 10.f);
	DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[2].Location, FColor::White, false, -1.f, 0, 10.f);
	DrawDebugLine(GetWorld(), HitPoints[2].Location, HitPoints[1].Location, FColor::Red, false, -1.f, 0, 10.f);
	DrawDebugLine(GetWorld(), HitPoints[1].Location, HitPoints[3].Location, FColor::White, false, -1.f, 0, 10.f);
	DrawDebugLine(GetWorld(), HitPoints[2].Location, HitPoints[3].Location, FColor::White, false, -1.f, 0, 10.f);

	DrawDebugLine(GetWorld(), (HitPoints[0].Location + HitPoints[3].Location) / 2, (HitPoints[0].Location + HitPoints[3].Location) / 2 + NewUpVector.GetSafeNormal() * 500, FColor::Green, false, -1.f, 0, 12.f);

	// Clamp angles so that  the ship cannot flip
	/*NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);*/
	//NewRotation.Yaw = GetActorRotation().Yaw;
	//NewRotation.Roll = 0;
	
	
	return NewRotation;
}


FRotator APlayerShipTesting::GetSurfaceNormalSimple() 
{
	const float RayCastLength = 4000.f;
	
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);
	
	for (int i{}; i < 4; i++)
	{
		FVector Start, End;
		
		switch (i)
		{
		case 0:
			Start = Thrust1->GetComponentLocation();
			End = Start + Thrust1->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 1:
			Start = Thrust2->GetComponentLocation();
			End = Start + Thrust2->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 2:
			Start = Thrust3->GetComponentLocation();
			End = Start + Thrust3->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 3:
			Start = Thrust4->GetComponentLocation();
			End = Start + Thrust4->GetComponentRotation().Vector() * RayCastLength;
			break;
		default:
			Start = FVector::ZeroVector;
		}
		
		FCollisionQueryParams CollisionParams;

		DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.f, 0, 12.f);
		
		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			// If raycast hit something
			if (HitPoints[i].bBlockingHit)
			{
				// Raycast visuals
				
				DrawDebugSphere(GetWorld(), HitPoints[i].Location, 60.f, 16, FColor::Blue);
			}
		}
		else
		{
			FRotator TempRot = GetActorRotation();
			TempRot.Pitch = FMath::FInterpTo(TempRot.Pitch, 0.f, GetWorld()->GetDeltaSeconds(), 0.8f);
			TempRot.Roll = 0.f;
			return TempRot;
		}
	}

	FVector NewUpVector = FVector::ZeroVector;
	for (int i{}; i < 4; i++)
	{
		NewUpVector += HitPoints[i].ImpactNormal;
	}

	FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());
	NewRotation.Yaw = GetActorRotation().Yaw;
	
	// Upwards line
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + NewUpVector * 1000, FColor::Green, false, -1.f, 0, 12.f);
	
	return NewRotation;
	// Clamp angles so that  the ship cannot flip
	/*NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;*/
}
float APlayerShipTesting::CustomInterp(float Current, float Target, float DeltaTime, float InterpSpeed)
{
	// Distance left to move
	const float Distance = Target - Current;

	// If distance is too small, just set the desired location (copied from UnrealMath.cpp)
	if( FMath::Square(Distance) < SMALL_NUMBER )
	{
		return Target;
	}

	// Where on the custom curve we should be (based off how far we have come)
	const float Alpha = FMath::Clamp<float>( FMath::Abs( Current / Target ), 0.f, 1.f );

	// How far we should move this tick
	float DeltaMove = Distance * DeltaTime * CustomCurve1->GetFloatValue( Alpha ) * InterpSpeed;

	// Return the next position
	return Current + DeltaMove;
}
float APlayerShipTesting::CustomInterp2(float Current, float Target, float DeltaTime, float InterpSpeed)
{
	// Distance left to move
	const float Distance = Target - Current;

	// If distance is too small, just set the desired location (copied from UnrealMath.cpp)
	if( FMath::Square(Distance) < SMALL_NUMBER )
	{
		return Target;
	}

	// Where on the custom curve we should be (based off how far we have come)
	const float Alpha = FMath::Clamp<float>( FMath::Abs( Current / Target ), 0.f, 1.f );

	// How far we should move this tick
	float DeltaMove = Distance * DeltaTime * CustomCurve2->GetFloatValue( Alpha ) * InterpSpeed;

	// Return the next position
	return Current + DeltaMove;
}
