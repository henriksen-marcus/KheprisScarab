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
#include "AnimGraphRuntime/Public/CommonAnimationTypes.h"
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
	
	MaxSpeedBoost = 1.2f;
	SpeedBoost = 1.f;
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
	SpringArm->CameraLagSpeed = 10.f; // Lower = More delay
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagMaxDistance = 5000.f;
	SpringArm->CameraRotationLagSpeed = 10.f;
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
	
	/*ThrustLocations.Reserve(4);
	ThrustLocations.Add(Thrust1);
	ThrustLocations.Add(Thrust2);
	ThrustLocations.Add(Thrust3);
	ThrustLocations.Add(Thrust4);*/

	/*ThrustLocations.Emplace(Thrust1);
	ThrustLocations.Emplace(Thrust2);
	ThrustLocations.Emplace(Thrust3);
	ThrustLocations.Emplace(Thrust4);*/
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	TargetLocation = GetActorLocation();
	/*UE_LOG(LogTemp, Warning, TEXT("Length: %d"), ThrustLocations.Num())
	for (int i{}; i < ThrustLocations.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pos: %s"), *ThrustLocations[i]->GetComponentLocation().ToString())
	}*/
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("Loc: %s"), *TargetLocation.ToString())

	FRotator SurfaceNormal = GetSurfaceNormal();
	FRotator NewRotation = GetActorRotation();
	float RotInterpSpeed = 5.f;
	
	// X and Y Movement
	//FRotator RotAmount = UKismetMathLibrary::MakeRotFromXZ(GetActorForwardVector(), GetActorUpVector());
	//FVector Result = RotAmount.RotateVector(LocalMove)*50;
	if (!RtRpl->IsSimulatingPhysics())
	{
		SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 10.f), true);
		//SetActorLocation(FMath::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 10.f));
	}
	AddActorLocalOffset(LocalMove * DeltaTime * 200.f * SpeedMultiplier, true);
	
	//AddActorLocalOffset(LocalMove * DeltaTime * 110.f * SpeedBoost * SpeedMultiplier, true);

	// Root Rotation

	// If we are close to 90 degrees, disable rotation interpolation
	/*NewRotation.Pitch = SurfaceNormal.Pitch;
	NewRotation.Yaw = SurfaceNormal.Yaw;
	NewRotation.Roll = SurfaceNormal.Roll;*/
	
	/*if (FMath::IsNearlyEqual(SurfaceNormal.Pitch, 90.f, 2.f) || FMath::IsNearlyEqual(SurfaceNormal.Pitch, -90.f, 2.f))
	{ NewRotation.Pitch = SurfaceNormal.Pitch; UE_LOG(LogTemp, Warning, TEXT("Pitch is 90 deg %f"), FMath::RandRange(0.f,1.f))}
	else
	{ NewRotation.Pitch = FMath::FInterpTo(NewRotation.Pitch, SurfaceNormal.Pitch, DeltaTime, RotInterpSpeed); }*/
	
	/*if (FMath::IsNearlyEqual(SurfaceNormal.Yaw, 90.f, 2.f) || FMath::IsNearlyEqual(SurfaceNormal.Yaw, -90.f, 2.f))
	{ NewRotation.Yaw = SurfaceNormal.Yaw; }
	else
	{ NewRotation.Yaw = FMath::FInterpTo(NewRotation.Yaw, SurfaceNormal.Yaw, DeltaTime, RotInterpSpeed); }*/

	/*if (FMath::IsNearlyEqual(SurfaceNormal.Roll, 90.f, 2.f) || FMath::IsNearlyEqual(SurfaceNormal.Roll, -90.f, 2.f))
	{ NewRotation.Roll = SurfaceNormal.Roll; }
	else
	{ NewRotation.Roll = FMath::FInterpTo(NewRotation.Roll, SurfaceNormal.Roll, DeltaTime, RotInterpSpeed); }*/

	if (
		FMath::IsNearlyEqual(SurfaceNormal.Pitch, 90.f, 3.f) || FMath::IsNearlyEqual(SurfaceNormal.Pitch, -90.f, 3.f) ||
		/*FMath::IsNearlyEqual(SurfaceNormal.Yaw, 90.f, 3.f) || FMath::IsNearlyEqual(SurfaceNormal.Yaw, -90.f, 3.f) ||*/
		FMath::IsNearlyEqual(SurfaceNormal.Roll, 90.f, 3.f) || FMath::IsNearlyEqual(SurfaceNormal.Roll, -90.f, 3.f)
		)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stop interp"))
		NewRotation = SurfaceNormal;
	}
	else
	{
		NewRotation = FMath::RInterpTo(NewRotation, SurfaceNormal, DeltaTime, 8.f);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Pitch: %f, Yaw: %f, Roll: %f"), NewRotation.Pitch, NewRotation.Yaw, NewRotation.Roll)
	SetActorRotation(NewRotation);
	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	// Cosmetic mesh rotation
	//BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));
	
	/** Springarm rotation */
	FRotator NewRot = SpringArm->GetRelativeRotation();
	NewRot = FMath::RInterpTo(NewRot, SpringArmRotTarget, DeltaTime, 5.f);
	NewRot.Roll = 0.f;
	SpringArm->SetRelativeRotation(NewRot);

	/** Camera effects */
	Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetCameraFOV, DeltaTime, 10.f));
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetSpringArmLength, DeltaTime, 10.f);

	//FallTime += DeltaTime;
	CameraCenteringTimer += DeltaTime;
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
	PlayerInputComponent->BindAction("UseItem", EInputEvent::IE_Pressed, this, &APlayerShip::Dash);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerShip::Crouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerShip::CrouchEnd);

	PlayerInputComponent->BindAction("ScrollUp", EInputEvent::IE_Pressed, this, &APlayerShip::CameraZoomIn);
	PlayerInputComponent->BindAction("ScrollDown", EInputEvent::IE_Pressed, this, &APlayerShip::CameraZoomOut);
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
	float InterpSpeed = bPitchHasInput ? 2.f : 0.5f;
	InterpSpeed = Value < 0.f ? InterpSpeed / 3.f : InterpSpeed;
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), InterpSpeed);
}


void APlayerShip::Turn(const float Value)
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


void APlayerShip::CameraYaw(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;
	
	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Yaw = bShouldReset ? 0.f : FMath::Clamp(SpringArm->GetRelativeRotation().Yaw + Value * 5.f, -80.f, 80.f);
}


void APlayerShip::CameraPitch(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;

	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Pitch = bShouldReset ? -20.f : FMath::Clamp(SpringArm->GetRelativeRotation().Pitch + Value * 10.f, -80.f, 0.f);
}


void APlayerShip::Dash() 
{
	if (bIsDashing) 
	{
		return;
	}

	TargetCameraFOV += 40.f;
	TargetSpringArmLength -= 800.f;
	SpeedBoost = MaxSpeedBoost;
	bIsDashing = true;

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::ResetDash, DashTimer, false);
}


void APlayerShip::ResetDash()
{
	TargetCameraFOV -= 40.f;
	TargetSpringArmLength += 800.f;
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

void APlayerShip::Crouch()
{
	TargetHeight /= 2.f;
}

void APlayerShip::CrouchEnd()
{
	TargetHeight *= 2.f;
}


void APlayerShip::CameraZoomIn()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength - 200.f, 1000.f, 3500.f);
}


void APlayerShip::CameraZoomOut()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength + 200.f, 1000.f, 3500.f);
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
	float RayCastLength = 2000.f;
	FRotator NewRotation = FRotator::ZeroRotator;
	FVector NewUpVector = FVector::ZeroVector;
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);

	// Used to determine if we are in the air - Counter == 4 -> Four raycast misses
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
			break;
		}
		
		FCollisionQueryParams CollisionParams;

		if (bEnableDebugLines)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.f, 0, 12.f);
		}
		
		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			if (bEnableDebugLines)
			{
				// Sphere at collision point
				DrawDebugSphere(GetWorld(), HitPoints[i].Location, 60.f, 16, FColor::Blue);
			}
		}
		else // If the raycast didn't hit anything within the raycast length
		{
			/*End.Z += 500;
			HitPoints[i].Location = End;*/
			Counter++;
		}
	}

	// If we are fully in-air, return to upright rotation and pitch downward gradually
	if (Counter == 4)
	{
		
		FRotator InAirRot = GetActorRotation();
		InAirRot.Pitch = FMath::FInterpTo(InAirRot.Pitch, -25.f, GetWorld()->GetDeltaSeconds(), 4.f);
		//InAirRot.Roll = UKismetMathLibrary::FInterpEaseInOut(InAirRot.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 0.2f);
		InAirRot.Roll = FMath::FInterpTo(InAirRot.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 15.f);
		FallSpeed = FMath::Lerp(1.f, 500.f, FallSpeed/500.f) + 1.f;
		//FallSpeed = FMath::InterpEaseIn(1.f, 300.f, FallSpeed/300.f, 0.8f);
		TargetLocation = GetActorLocation();
		TargetLocation.Z -= FallSpeed;
		//RtRpl->SetSimulatePhysics(true);
		//RtRpl->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		UE_LOG(LogTemp, Warning,TEXT("FallSpeed! %f"), FallSpeed)
		// Yaw remains unchanged, let the player have some control
		return InAirRot;
	}
	else if (Counter)
	{
		if (!HitPoints[0].bBlockingHit && !HitPoints[1].bBlockingHit)
		{
			TargetLocation = GetActorLocation();
			return GetActorRotation();
		}
		
		const float Length = 1200.f;
		UE_LOG(LogTemp, Warning, TEXT("Not all points hit"))
		for (int i{}; i < 4; i++)
		{
			if (!HitPoints[i].bBlockingHit)
			{
				FVector EndPoint;
				switch (i)
				{
				case 0:
					EndPoint = Thrust1->GetComponentLocation() + Thrust1->GetComponentRotation().Vector() * Length;
					break;
				case 1:
					EndPoint = Thrust2->GetComponentLocation() + Thrust2->GetComponentRotation().Vector() * Length;
					break;
				case 2:
					EndPoint = Thrust3->GetComponentLocation() + Thrust3->GetComponentRotation().Vector() * Length;
					break;
				case 3:
					EndPoint = Thrust4->GetComponentLocation() + Thrust4->GetComponentRotation().Vector() * Length;
					break;
				}
				HitPoints[i].Location = EndPoint;
			}
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
		NewUpVector = (CrossA + CrossD);

		// Get the target vector in world space, relative to the hit points casted from the player. This detaches us from world "up" & "down".
		{
			TargetLocation = (HitPoints[0].Location + HitPoints[1].Location + HitPoints[2].Location + HitPoints[3].Location) / 4;
			TargetLocation += NewUpVector.GetSafeNormal() * TargetHeight;
		
			if (bEnableDebugLines) { DrawDebugSphere(GetWorld(), TargetLocation, 100.f, 16, FColor::Emerald); }
		}
	}
	else // If all points hit
	{
		FallSpeed = 0.f;
		RtRpl->SetSimulatePhysics(false);
			
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
		NewUpVector = (CrossA + CrossD);

		// Math visualization
		if (bEnableDebugLines)
		{
			// Cross product above A
			DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[0].Location + CrossA.GetSafeNormal() * 1000, FColor::Red, false, -1.f, 0, 12.f);
		
			//Cross product above D
			DrawDebugLine(GetWorld(), HitPoints[3].Location, HitPoints[3].Location + CrossD.GetSafeNormal() * 1000, FColor::Blue, false, -1.f, 0, 12.f);
		
			//Lines between points
			DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[1].Location, FColor::White, false, -1.f, 0, 10.f);
			DrawDebugLine(GetWorld(), HitPoints[0].Location, HitPoints[2].Location, FColor::White, false, -1.f, 0, 10.f);
			DrawDebugLine(GetWorld(), HitPoints[2].Location, HitPoints[1].Location, FColor::Red, false, -1.f, 0, 10.f);
			DrawDebugLine(GetWorld(), HitPoints[1].Location, HitPoints[3].Location, FColor::White, false, -1.f, 0, 10.f);
			DrawDebugLine(GetWorld(), HitPoints[2].Location, HitPoints[3].Location, FColor::White, false, -1.f, 0, 10.f);

			//Center line representing the combination of the cross product vectors (new up vector)
			DrawDebugLine(GetWorld(), (HitPoints[0].Location + HitPoints[3].Location) / 2, (HitPoints[0].Location + HitPoints[3].Location) / 2 + NewUpVector.GetSafeNormal() * 500, FColor::Green, false, -1.f, 0, 12.f);	
		}

		// Get the target vector in world space, relative to the hit points casted from the player. This detaches us from world "up" & "down".
		{
			TargetLocation = (HitPoints[0].Location + HitPoints[1].Location + HitPoints[2].Location + HitPoints[3].Location) / 4;
			TargetLocation += NewUpVector.GetSafeNormal() * TargetHeight;
		
			if (bEnableDebugLines) { DrawDebugSphere(GetWorld(), TargetLocation, 100.f, 16, FColor::Emerald); }
		}
	}
	
	

	if (NewRotation == FRotator::ZeroRotator)
	{
		NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());
	}

	// Warning! If arrow components are tilted, the 2D projection on the ground will be skewed, making wrong forward vector.
	//FRotator NewRotation = UKismetMathLibrary::MakeRotationFromAxes(-A_C.GetSafeNormal(), A_B, NewUpVector.GetSafeNormal());
	

	//Find a way to negate the yaw change!
	
	return NewRotation;
}



FRotator APlayerShip::GetSurfaceNormalSimple() 
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
	
	TargetZ2 = 0.f;
	for (auto i : HitPoints)
	{
		TargetZ2 += i.ImpactPoint.Z + TargetHeight;
	}
	TargetZ2 /= 4;
	
	return NewRotation;
	// Clamp angles so that  the ship cannot flip
	/*NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;*/
}


/*
float APlayerShip::GetTargetZ()
{
	/** Raycasting - Keeping the ship afloat #1#
	FHitResult OutHit;
	FVector Start = GetActorLocation();
	FVector End = Start;
	End.Z -= 1500.f;
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, -1.f, 0, 18.f);

	if (!GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("Raycast did not hit anything. FallSpeed is: %f"), 300.f + FallTime * 100)
		return Start.Z - (500.f + FallTime * 800);
	}
	else if (OutHit.bBlockingHit)
	{
		/*if (Start.Z - OutHit.ImpactPoint.Z < 350.f)
		{
			bLowThreshold = true;
			return OutHit.ImpactPoint.Z + 350.f;
		}
		FallTime = 0.f;#1#
		return OutHit.ImpactPoint.Z + TargetHeight;
	}
	else
	{
		FallTime = 0.f;
		return GetActorLocation().Z;
	}
}
*/


float APlayerShip::GetTargetZ()
{
	const float RayCastLength = 1800.f;
	
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
			break;
		}
		
		FCollisionQueryParams CollisionParams;
		
		// Raycast, cancel operation if we dont get a hit
		if (!GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			/*switch (i)
			{
			case 0:
				HitPoints[0].Location = Start + Thrust1->GetComponentRotation().Vector() * TargetHeight - 500;
				break;
			case 1:
				HitPoints[1].Location = Start + Thrust2->GetComponentRotation().Vector() * TargetHeight - 500;
				break;
			case 2:
				HitPoints[2].Location = Start + Thrust3->GetComponentRotation().Vector() * TargetHeight - 500;
				break;
			case 3:
				HitPoints[3].Location = Start + Thrust4->GetComponentRotation().Vector() * TargetHeight - 500;
				break;
			default:
				break;
			}*/
			
			//return GetActorLocation().Z;
			HitPoints[i].Location = End;
			Counter++;
		}
	}

	const float TargetZ = ((HitPoints[0].Location.Z + HitPoints[1].Location.Z + HitPoints[2].Location.Z + HitPoints[3].Location.Z) / 4) + TargetHeight;
	//UE_LOG(LogTemp, Warning, TEXT("TargetZ: %f"), TargetZ)
	if (Counter == 4)
	{
		FallSpeed = CustomInterp2(FallSpeed, 1000.f, GetWorld()->GetDeltaSeconds());
		UE_LOG(LogTemp, Warning, TEXT("FallSpeed: %f"), FallSpeed);
		return TargetZ - FallSpeed;
	}
	else
	{
		FallSpeed = 10.f;
		return TargetZ;
	}
}


/*FVector APlayerShip::GetTargetLocation()
{
	return FVector::ZeroVector;
}*/


float APlayerShip::CustomInterp(float Current, float Target, float DeltaTime, float InterpSpeed)
{
	if (!CustomCurve1) { return 0; }
	
	// Distance left to move
	const float Distance = Target - Current;
	//FMath::InterpEaseIn()
	// If distance is too small, just set the desired location (copied from UnrealMath.cpp)
	if( FMath::Square(Distance) < SMALL_NUMBER )
	{
		return Target;
	}

	// Where on the custom curve we should be (based off how far we have come)
	//float Alpha = FMath::Clamp<float>( FMath::Abs( Current / Target ), 0.f, 1.f );
	float Alpha = Distance > 0.f ? Current / Target : Distance;
	UE_LOG(LogTemp, Warning, TEXT("Alpha: %f"), Alpha)

	// How far we should move this tick
	const float DeltaMove = Distance * DeltaTime * CustomCurve1->GetFloatValue( Alpha ) * InterpSpeed;

	// Return the next position
	return Current + DeltaMove;
}


float APlayerShip::CustomInterp2(float Current, float Target, float DeltaTime, float InterpSpeed)
{
	if (!CustomCurve2) { return 0; }
	
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


// Old code

/*
FRotator APlayerShip::GetSurfaceNormal3P()
{
	// We need three points and two vectors to determine the cross product and get the new up vector
	FVector A = FVector::ZeroVector, B = FVector::ZeroVector, C = FVector::ZeroVector, NewUpVector = FVector::ZeroVector;
	
	float RayCastLength = 1500.f;
	
	for (int i{}; i < 3; i++)
	{
		FHitResult OutHit;
		FVector Start = ThrustLocations[i]->GetComponentLocation();
		FVector End = Start;
		End.Z -= RayCastLength;
		FCollisionQueryParams CollisionParams;
		FVector downvec = -GetActorUpVector();
		FVector temp2 = Start.GetSafeNormal();
		// Raycast visualization, red line
		//DrawDebugLine(GetWorld(), Start, Start + (-GetActorUpVector() * 1000.f), FColor::Red, false, -1.f, 0, 12.f);
		//DrawDebugLine(GetWorld(), Start + (Start.GetSafeNormal() * 1000.f), Start + (Start.GetSafeNormal() * 200.f), FColor::Green, false, -1.f, 0, 12.f);

		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			// If raycast hit something
			if (OutHit.bBlockingHit)
			{
				//DrawDebugSphere(GetWorld(), OutHit.Location, 60.f, 16, FColor::Blue);

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
	if (!A.IsZero() && !B.IsZero() && !C.IsZero())
	{
		FVector VecAb, VecAc;
		
		VecAb = B - A;
		VecAc = C - A;

		NewUpVector = FVector::CrossProduct(VecAb, VecAc);
	}
	else
	{
		if (A.IsZero())
		{
			A = ThrustLocations[0]->GetComponentLocation();
			A.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), A, 80.f, 16, FColor::Orange);
			
			FVector VecAb = B - A;
			FVector VecAc = C - A;
			NewUpVector = FVector::CrossProduct(VecAb, VecAc);
		}
		if (B.IsZero())
		{
			B = ThrustLocations[0]->GetComponentLocation();
			B.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), B, 80.f, 16, FColor::Orange);
			
			FVector VecBa = A - B;
			FVector VecBc = C - B;
			NewUpVector = FVector::CrossProduct(VecBc, VecBa);
		}
		if (C.IsZero())
		{
			C = ThrustLocations[0]->GetComponentLocation();
			C.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), C, 80.f, 16, FColor::Orange);
			
			FVector VecCa = B - C;
			FVector VecCb = C - C;
			NewUpVector = FVector::CrossProduct(VecCa, VecCb);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error in PlayerShip::GetSurfaceNormal: This else statement should not be triggered."))
			return GetActorRotation();
		}
	}

	// Upwards line
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 1000, FColor::Green, false, -1.f, 0, 12.f);
	
	NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());

	// Clamp angles so that  the ship cannot flip
	NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;
}
*/

/*
FRotator APlayerShip::GetSurfaceNormal3P() // Edited so that we don't use tarray
{
	// We need three points and two vectors to determine the cross product and get the new up vector
	FVector A = FVector::ZeroVector, B = FVector::ZeroVector, C = FVector::ZeroVector, NewUpVector = FVector::ZeroVector;
	
	float RayCastLength = 1500.f;
	
	for (int i{}; i < 3; i++)
	{
		FHitResult OutHit;
		FVector Start;
		
		switch (i)
		{
		case 0:
			Start = Thrust1->GetComponentLocation();
			break;
		case 1:
			Start = Thrust2->GetComponentLocation();
			break;
		case 2:
			Start = Thrust3->GetComponentLocation();
			break;
		default:
			Start = FVector::ZeroVector;
		}
		
		FVector End = Start;
		End.Z -= RayCastLength;
		FCollisionQueryParams CollisionParams;

		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams))
		{
			// If raycast hit something
			if (OutHit.bBlockingHit)
			{
				//DrawDebugSphere(GetWorld(), OutHit.Location, 60.f, 16, FColor::Blue);

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
	if (!A.IsZero() && !B.IsZero() && !C.IsZero())
	{
		FVector VecAb, VecAc;
		
		VecAb = B - A;
		VecAc = C - A;

		NewUpVector = FVector::CrossProduct(VecAb, VecAc);
	}
	else
	{
		if (A.IsZero())
		{
			A = Thrust1->GetComponentLocation();
			A.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), A, 80.f, 16, FColor::Orange);
			
			FVector VecAb = B - A;
			FVector VecAc = C - A;
			NewUpVector = FVector::CrossProduct(VecAb, VecAc);
		}
		else if (B.IsZero())
		{
			B = Thrust2->GetComponentLocation();
			B.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), B, 80.f, 16, FColor::Orange);
			
			FVector VecBa = A - B;
			FVector VecBc = C - B;
			NewUpVector = FVector::CrossProduct(VecBc, VecBa);
		}
		else if (C.IsZero())
		{
			C = Thrust3->GetComponentLocation();
			C.Z -= RayCastLength;
			DrawDebugSphere(GetWorld(), C, 80.f, 16, FColor::Orange);
			
			FVector VecCa = B - C;
			FVector VecCb = C - C;
			NewUpVector = FVector::CrossProduct(VecCa, VecCb);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error in PlayerShip::GetSurfaceNormal: This else statement should not be triggered."))
			return GetActorRotation();
		}
	}

	// Upwards line
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 1000, FColor::Green, false, -1.f, 0, 12.f);
	
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());

	// Clamp angles so that  the ship cannot flip
	NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;
}
*/
