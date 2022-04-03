// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShip.h"

//#include <dsound.h>

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "ToolContextInterfaces.h"
#include "AnimGraphRuntime/Public/CommonAnimationTypes.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Matinee/MatineeInterface.h"


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
	
	/*ThrustLocations.Reserve(4);
	ThrustLocations.Add(Thrust1);
	ThrustLocations.Add(Thrust2);
	ThrustLocations.Add(Thrust3);
	ThrustLocations.Add(Thrust4);*/

	/*ThrustLocations.Emplace(Thrust1);
	ThrustLocations.Emplace(Thrust2);
	ThrustLocations.Emplace(Thrust3);
	ThrustLocations.Emplace(Thrust4);*/
	
	MoveComp = CreateDefaultSubobject<UHoveringMovementComponent>(TEXT("HoverComponentMoveStuffLol"));
}


void APlayerShip::BeginPlay()
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
}

//theta = pi/2 * alpha


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const FVector OldPos = GetActorLocation();
	
	const FRotator SurfaceNormal = GetSurfaceNormal();
	FRotator NewRotation = GetActorRotation();
	
	//MoveComp->MoveUpdatedComponent(GetActorRotation().RotateVector(LocalMove), GetActorRotation(), true);
	
	// Movement
	SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 10.f), true);
	AddActorLocalOffset(LocalMove * DeltaTime * 200.f * SpeedMultiplier * SpeedBoost, true);

	// Jumping functionality
	if(bIsJumping && JumpCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitTH: %f"), InitialTargetHeight)
		UE_LOG(LogTemp, Warning, TEXT("TH: %f"), TargetHeight)
		TargetHeight = InitialTargetHeight * (JumpCurve->GetFloatValue ( JumpTimer/3.f ) + 1.f);
		//AddActorLocalOffset(GetActorUpVector() *  200.f * JumpCurve->GetFloatValue ( JumpTimer/4.f ));

		if (JumpTimer <= 0.4f)
		{
			LocationInterpolationSpeed = 20.f;
		}
		else
		{
			LocationInterpolationSpeed = 10.f;
		}
		
		JumpTimer += DeltaTime;
		
	}

	// Engine dynamic audio
	const FVector Diff = OldPos - GetActorLocation();
	const float Speed = FMath::Abs(Diff.Size());
	
	// Interp to smooth out speed/audio fluctuations
	AudioComp->PitchMultiplier = FMath::FInterpTo(AudioComp->PitchMultiplier, (CustomCurve2->GetFloatValue(Speed/200.f) + 1), DeltaTime, 1.5f);
	AudioComp->SetPitchMultiplier(AudioComp->PitchMultiplier);
	
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
		FMath::IsNearlyEqual(SurfaceNormal.Pitch, 90.f, 1.f) || FMath::IsNearlyEqual(SurfaceNormal.Pitch, -90.f, 1.f) //||
		/*FMath::IsNearlyEqual(SurfaceNormal.Yaw, 90.f, 3.f) || FMath::IsNearlyEqual(SurfaceNormal.Yaw, -90.f, 3.f) ||*/
		/*FMath::IsNearlyEqual(SurfaceNormal.Roll, 90.f, 1.f) || FMath::IsNearlyEqual(SurfaceNormal.Roll, -90.f, 1.f)*/
		)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stop interp"))
		NewRotation = SurfaceNormal;
	}
	else
	{
		NewRotation = FMath::RInterpTo(NewRotation, SurfaceNormal, DeltaTime, 8.f);
	}

	SetActorRotation(NewRotation);
	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	// Cosmetic mesh rotation
	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));
	
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
	const float TargetPitch = bPitchHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

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


void APlayerShip::Jump()
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
	float RayCastLength = 4000.f;
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

	// If we are fully in-air
	if (Counter == 4)
	{
		// Return to upright rotation
		FRotator InAirRot = GetActorRotation();
		InAirRot.Pitch = FMath::FInterpTo(InAirRot.Pitch, -25.f, GetWorld()->GetDeltaSeconds(), 5.f);
		InAirRot.Roll = FMath::FInterpTo(InAirRot.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 20.f);

		// Increase fall speed
		FallSpeed = FMath::InterpEaseIn(1.f, 1500.f, FMath::Clamp(FallTimer/3, 0.f, 1.f), 1.2f);
		TargetLocation = GetActorLocation();
		TargetLocation.Z -= FallSpeed;
		FallTimer += GetWorld()->GetDeltaSeconds();
		UE_LOG(LogTemp, Warning,TEXT("FallSpeed! %f"), FallSpeed)
		
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
		// Impact camera shake
		if (FallSpeed >= 1000.f && CamShake)
		{
			UE_LOG(LogTemp, Warning, TEXT("Camera Shake!"))
			UGameplayStatics::PlayWorldCameraShake(Camera, CamShake, Camera->GetComponentLocation(), 100.f, 1000.f);
		}
		
		FallTimer = 0.f;
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
	
	return NewRotation;
	// Clamp angles so that  the ship cannot flip
	/*NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;*/
}


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
