// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipPhysics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerShipPhysics::APlayerShipPhysics()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	SetRootComponent(Root);

	Root->SetSimulatePhysics(true);
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);

	//BaseMesh->SetSimulatePhysics(true);
	
	InitialTargetHeight = TargetHeight;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	//SpringArm->SetUsingAbsoluteRotation(false);
	//SpringArm->bUsePawnControlRotation = true;
	SpringArm->TargetArmLength = TargetSpringArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 20.f; // Lower = More delay
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagMaxDistance = 2000.f;
	SpringArm->CameraRotationLagSpeed = 5.f;
	SpringArm->bDoCollisionTest = true;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(12.f, 0.f, 0.f));
	
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComp->SetupAttachment(GetRootComponent());
	
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
	
	MoveComp = CreateDefaultSubobject<UHoveringMovementComponent>(TEXT("HoverComponentMoveStuffLol"));
}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
	TargetLocation = GetActorLocation();
	AudioComp->Deactivate();
	
	if (StartSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), StartSound, 0.2f);
		
		FTimerHandle TH_BeginPlay;
		FTimerDelegate TD_BeginPlay;
		// Lambda expression
		
		TD_BeginPlay.BindLambda([&]{ AudioComp->FadeIn(0.8f); AudioComp->PitchMultiplier = 1.f; });
		GetWorld()->GetTimerManager().SetTimer(TH_BeginPlay, TD_BeginPlay, 0.6f, false);
	}
	else
	{
		AudioComp->Activate();
	}
	
	// Times 100, because unreal uses cm measurement
	ShipWeight = Root->GetMass() * 100 * 9.80665f;
}


void APlayerShipPhysics::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	const FRotator SurfaceNormal = GetSurfaceNormal();
	FRotator NewRotation = GetActorRotation();
	
	/** Ship movement - Changed to physics based */

	// Local forwards force
	Root->AddForce(GetActorForwardVector() * Force.X);
	
	// Local up force
	//Root->AddForce(GetActorUpVector() * HoverForceCurve->GetFloatValue(DistanceAboveGround / 1000.f))
	
	// World up force
	//Root->AddForce(FVector::UpVector * ShipWeight);

	//Root->AddTorqueInRadians(GetActorRotation().RotateVector(RForce));

	// Constantly decrease angular velocity
	Root->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	/*FVector Resistance = FVector::ZeroVector;
	Resistance = Root->GetPhysicsLinearVelocity() * -1000.f;
	Root->AddForce(Resistance);*/

	
	// Engine dynamic audio
	const float Speed = Root->GetComponentVelocity().Size();

	if (bLogSpeed)
	{
		UE_LOG(LogTemp,Warning,TEXT("Speed is: %f"), Speed);
	}

	if (AudioComp && CustomCurve2)
	{
		// Interp to smooth out speed/audio fluctuations
		AudioComp->PitchMultiplier = FMath::FInterpTo(AudioComp->PitchMultiplier, (CustomCurve2->GetFloatValue(Speed/200.f) + 1), DeltaTime, 1.5f);
		AudioComp->SetPitchMultiplier(AudioComp->PitchMultiplier);
	}
	
	if (
		FMath::IsNearlyEqual(SurfaceNormal.Pitch, 90.f, 1.f) || FMath::IsNearlyEqual(SurfaceNormal.Pitch, -90.f, 1.f) //||
		/*FMath::IsNearlyEqual(SurfaceNormal.Yaw, 90.f, 3.f) || FMath::IsNearlyEqual(SurfaceNormal.Yaw, -90.f, 3.f) ||*/
		/*FMath::IsNearlyEqual(SurfaceNormal.Roll, 90.f, 1.f) || FMath::IsNearlyEqual(SurfaceNormal.Roll, -90.f, 1.f)*/
		)
	{
		NewRotation = SurfaceNormal;
	}
	else
	{
		NewRotation = FMath::RInterpTo(NewRotation, SurfaceNormal, DeltaTime, 8.f);
	}

	//SetActorRotation(NewRotation);
	//AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
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
	
	CameraCenteringTimer += DeltaTime;
}


void APlayerShipPhysics::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis("Forward", this, &APlayerShipPhysics::Forward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShipPhysics::Turn);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShipPhysics::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShipPhysics::CameraYaw);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Jump);
	PlayerInputComponent->BindAction("UseItem", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Dash);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Crouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerShipPhysics::CrouchEnd);

	PlayerInputComponent->BindAction("ScrollUp", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::CameraZoomIn);
	PlayerInputComponent->BindAction("ScrollDown", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::CameraZoomOut);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //


void APlayerShipPhysics::Forward(const float Value)
{
	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	const float TargetPitch = bPitchHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	Force.X = Value * 10.f * ShipWeight * SpeedBoost * SpeedMultiplier;
}

void APlayerShipPhysics::Turn(const float Value)
{
	// Determine if there is input
	bRollHasInput = !(Value == 0);

	// Roll
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	// Yaw
	YawMove = FMath::FInterpTo(YawMove,  Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);

	RForce.Z = Value * ShipWeight * 500.f;
}

void APlayerShipPhysics::CameraYaw(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;
	
	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Yaw = bShouldReset ? 0.f : FMath::Clamp(SpringArm->GetRelativeRotation().Yaw + Value * 5.f, -80.f, 80.f);
}

void APlayerShipPhysics::CameraPitch(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;

	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	SpringArmRotTarget.Pitch = bShouldReset ? -20.f : FMath::Clamp(SpringArm->GetRelativeRotation().Pitch + Value * 10.f, -80.f, 0.f);
}

void APlayerShipPhysics::Dash() 
{
	if (bIsDashing) 
	{
		return;
	}

	static float CamFovChange = 30.f;
	static float SpringArmChange = 800.f;

	TargetCameraFOV += CamFovChange;
	TargetSpringArmLength -= SpringArmChange;
	SpeedBoost = MaxSpeedBoost;
	bIsDashing = true;

	if (BoostSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BoostSound, 0.5f);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	// Lambda expression
	TimerDelegate.BindLambda([&]
		{
			TargetCameraFOV -= CamFovChange;
			TargetSpringArmLength += SpringArmChange;
			SpeedBoost = 1.f;
			bIsDashing = false;
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, DashTimer, false);
}

void APlayerShipPhysics::Jump()
{
	if (bIsJumping)
	{
		return;
	}
	bIsJumping = true;
	JumpTimer = 0.f;

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	
	// Lambda expression
	TimerDelegate.BindLambda([&]
		{
			bIsJumping = false;
		});

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 3.f, false);
}

void APlayerShipPhysics::Crouch()
{
	TargetHeight /= 2.f;
}

void APlayerShipPhysics::CrouchEnd()
{
	TargetHeight *= 2.f;
}

void APlayerShipPhysics::CameraZoomIn()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength - 200.f, 1000.f, 3500.f);
}

void APlayerShipPhysics::CameraZoomOut()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength + 200.f, 1000.f, 3500.f);
}

void APlayerShipPhysics::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }

}


FRotator APlayerShipPhysics::GetSurfaceNormal()
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
			//Root->AddForceAtLocation(FVector::UpVector*ShipWeight, Start);
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

	// If we are fully in-air
	if (Counter == 4)
	{
		// Return to upright rotation
		FRotator InAirRot = GetActorRotation();
		InAirRot.Pitch = FMath::FInterpTo(InAirRot.Pitch, -25.f, GetWorld()->GetDeltaSeconds(), 5.f);
		InAirRot.Roll = FMath::FInterpTo(InAirRot.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 20.f);
		// Yaw remains unchanged, let the player have some control
		return InAirRot;
	}
	else if (Counter) // If only one/some of the raycasts did not hit
	{
		FallTimer = 0.f;
		//FallSpeed = 0.f;
		if (!HitPoints[0].bBlockingHit && !HitPoints[1].bBlockingHit)
		{
			TargetLocation = GetActorLocation();
			return GetActorRotation();
		}
		
		const float Length = 2000.f;
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
		// Impact camera shake
		if (FallSpeed >= 1000.f && CamShake)
		{
			UGameplayStatics::PlayWorldCameraShake(Camera, CamShake, Camera->GetComponentLocation(), 100.f, 1000.f);
		}
		
		FallTimer = 0.f;
		FallSpeed = 0.f;
			
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
