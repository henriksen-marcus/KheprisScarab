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
	
	Thrust1->SetRelativeLocationAndRotation(FVector(300.f, -300.f, 0.f), FRotator(-60.f, -45.f, 0.f));
	Thrust2->SetRelativeLocationAndRotation(FVector(300.f, 300.f, 0.f), FRotator(-60.f, 45.f, 0.f));
	Thrust3->SetRelativeLocationAndRotation(FVector(-400.f, -300.f, 0.f), FRotator(-120.f, 45.f, 0.f));
	Thrust4->SetRelativeLocationAndRotation(FVector(-400.f, 300.f, 0.f), FRotator(-120.f, -45.f, 0.f));
	
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
	/*UE_LOG(LogTemp, Warning, TEXT("Length: %d"), ThrustLocations.Num())
	for (int i{}; i < ThrustLocations.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pos: %s"), *ThrustLocations[i]->GetComponentLocation().ToString())
	}*/
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Z Movement
	FVector CurLoc = GetActorLocation();
	const float TargetZ = GetTargetZ();
	if (bLowThreshold)
	{
		CurLoc.Z = TargetZ+1;
		bLowThreshold = false;
		UE_LOG(LogTemp, Warning, TEXT("Low Threshold!"))
	}
	else
	{
		CurLoc.Z = FMath::InterpEaseInOut(CurLoc.Z, TargetZ, DeltaTime, 0.6f);
	}
	SetActorLocation(CurLoc);
	
	// X and Y Movement
	AddActorLocalOffset(LocalMove * DeltaTime * 110.f * SpeedBoost * SpeedMultiplier, true);

	// Root Rotation
	//FRotator TargetRotation = GetSurfaceNormal3P();
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), GetSurfaceNormal4P(), DeltaTime, 2.f));
	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));
	
	// Cosmetic mesh rotation
	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));
	
	/** Springarm rotation */
	FRotator NewRot = SpringArm->GetRelativeRotation();
	NewRot.Pitch = FMath::InterpEaseInOut(NewRot.Pitch, SpringArmRotTarget.Pitch, DeltaTime, 0.6f);
	NewRot.Yaw = FMath::InterpEaseInOut(NewRot.Yaw, SpringArmRotTarget.Yaw, DeltaTime, 0.6f);
	//NewRot.Roll = 0;
	SpringArm->SetRelativeRotation(NewRot);

	/** Camera effects */
	Camera->SetFieldOfView(FMath::FInterpTo(Camera->FieldOfView, TargetCameraFOV, DeltaTime, 10.f));
	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetSpringArmLength, DeltaTime, 10.f);

	FallTime += DeltaTime;
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

	// Roll
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	// Yaw
	YawMove = FMath::FInterpTo(YawMove,  Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);
}


void APlayerShip::CameraYaw(const float Value)
{
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	
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
	SpringArmRotTarget.Pitch = bShouldReset ? -30.f : FMath::Clamp(SpringArm->GetRelativeRotation().Pitch + Value * 10.f, -80.f, 0.f);
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


void APlayerShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }

}


void APlayerShip::EscPressed()
{
	FGenericPlatformMisc::RequestExit(false);
}


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


FRotator APlayerShip::GetSurfaceNormal4P()
{
	float RayCastLength = 4000.f;
	
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);
	//FVector NewUpVector = FVector::ZeroVector;
	
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
			TempRot.Pitch = 0.f;
			TempRot.Roll = 0.f;
			return TempRot;
		}
	}

	// Inserted code

	FVector NUV = FVector::ZeroVector;
	for (int i{}; i < 4; i++)
	{
		NUV += HitPoints[i].ImpactNormal;
	}
	NUV /= 4;
	FRotator NR = UKismetMathLibrary::MakeRotFromZX(NUV, GetActorForwardVector());
	NR.Yaw = GetActorRotation().Yaw;
	return NR;
	
	// End of inserted code

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
	const FVector NewUpVector = (CrossA + CrossD) / 2;


	FRotator NewRotation = UKismetMathLibrary::MakeRotFromZX(NewUpVector, GetActorForwardVector());

	// Upwards line
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 1000, FColor::Green, false, -1.f, 0, 12.f);

	// Clamp angles so that  the ship cannot flip
	NewRotation.Pitch = FMath::ClampAngle(NewRotation.Pitch, -40.f, 40.f);
	NewRotation.Roll = FMath::ClampAngle(NewRotation.Roll, -40.f, 40.f);
	NewRotation.Yaw = GetActorRotation().Yaw;

	return NewRotation;
}


float APlayerShip::GetTargetZ()
{
	/** Raycasting - Keeping the ship afloat */
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
		if (Start.Z - OutHit.ImpactPoint.Z < 350.f)
		{
			bLowThreshold = true;
			return OutHit.ImpactPoint.Z + 350.f;
		}
		FallTime = 0.f;
		return OutHit.ImpactPoint.Z + 1000.f;
	}
	else
	{
		FallTime = 0.f;
		return GetActorLocation().Z;
	}
}
