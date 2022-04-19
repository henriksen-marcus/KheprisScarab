// Fill out your copyright notice in the Description page of Project Settings.


#include "HoveringMovementComponent.h"
#include "PlayerShipPhysics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UHoveringMovementComponent::UHoveringMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UHoveringMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ShipRef = Cast<APlayerShipPhysics>(GetOwner());
	
	if (!IsValid(ShipRef))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HoveringMovementComponent.cpp: Could not get ShipRef!"), true, FVector2D(20.f, 20.f));
	}

	ShipWeight = ShipRef->ShipWeight;
	Gravity = ShipRef->Gravity;
	
}


// Called every frame
void UHoveringMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Local + Global gravity, added a little more force the the opposing (local) force
	const FVector CombinedGravity = ( FVector::DownVector - (ShipRef->GetActorUpVector() * 1.1f) ).GetSafeNormal();
	ShipRef->Root->AddForce(CombinedGravity * Gravity);

	// Local forwards force
	ShipRef->Root->AddForce(ShipRef->GetActorForwardVector() * Force.X);

	MovementUpdate();

	// Limit speed
	if (ShipRef->Root->GetPhysicsLinearVelocity().Size() > 20000.f && !bIsDashing)
	{
		ShipRef->Root->SetPhysicsLinearVelocity(ShipRef->Root->GetPhysicsLinearVelocity().GetSafeNormal() * 20000.f);
	}

	if (bIsJumping)
	{
		ShipRef->Root->AddForce(ShipRef->GetActorUpVector() * Gravity * 20);
	}

	UE_LOG(LogTemp, Warning, TEXT("Movementcomponent working."))
}

void UHoveringMovementComponent::Forward(const float Value)
{
	// Determine if there is input
	bool bForwardHasInput = !(Value == 0);
	
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	const float TargetPitch = bForwardHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(ShipRef->BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	Force.X = Value ? (Value * 10.f * ShipWeight * SpeedBoost * ForwardsSpeed) : FMath::FInterpTo(Force.X, 0.f, GetWorld()->GetDeltaSeconds(), 5.f);
	//ActualSpeed = Value > 0.f ? ForwardsSpeed : FMath::FInterpTo(ActualSpeed, 0.f, GetWorld()->GetDeltaSeconds(), 0.5f);
	UE_LOG(LogTemp, Warning, TEXT("AcutalSpeedX: %f"), ActualSpeed)
}

void UHoveringMovementComponent::Turn(const float Value)
{
	// Determine if there is input
	bool bRollHasInput = !(Value == 0);

	// Roll
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(ShipRef->BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	// Yaw
	YawMove = FMath::FInterpTo(YawMove,  Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);
}


void UHoveringMovementComponent::MovementUpdate()
{
	float RayCastLength = InitialTargetHeight * 2 + 500;
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);

	// Used to determine if we are in the air - Counter == 4 -> Four raycast misses
	int32 Counter{};

	// Start raycast
	for (int i{}; i < 4; i++)
	{
		FVector Start, End;
		
		switch (i)
		{
		case 0:
			Start = ShipRef->Thrust1->GetComponentLocation();
			End = Start + ShipRef->Thrust1->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 1:
			Start = ShipRef->Thrust2->GetComponentLocation();
			End = Start + ShipRef->Thrust2->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 2:
			Start = ShipRef->Thrust3->GetComponentLocation();
			End = Start + ShipRef->Thrust3->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 3:
			Start = ShipRef->Thrust4->GetComponentLocation();
			End = Start + ShipRef->Thrust4->GetComponentRotation().Vector() * RayCastLength;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Error in UHoveringMovementComponent::MovementUpdate()"))
			break;
		}
		
		FCollisionQueryParams CollisionParams;

		if (bEnableDebugLines)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.f, 0, 5.f);
		}
		
		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			if (bEnableDebugLines)
			{
				// Sphere at collision point
				DrawDebugSphere(GetWorld(), HitPoints[i].Location, 60.f, 12, FColor::Blue);
			}
		}
		else
		{
			// If the raycast didn't hit anything within the raycast length
			Counter++;
		}
	}
	// End raycast

	// Decide what to do based on how many raycasts were hits
	for (int i{}; i < HitPoints.Num(); i++)
	{
		if (HitPoints[i].bBlockingHit)
		{
			switch (i)
			{
			case 0:
				AddForce(HitPoints[0].Location, 1);
				break;
			case 1:
				AddForce(HitPoints[1].Location, 2);
				break;
			case 2:
				AddForce(HitPoints[2].Location, 3);
				break;
			case 3:
				AddForce(HitPoints[3].Location, 4);
				break;
			default:
				break;
			}
		}
	}
	
	if (Counter == 4)
	{
		// Ship is in the air
		FRotator CurrentRotation = ShipRef->GetActorRotation();
		CurrentRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, -5.f, GetWorld()->GetDeltaSeconds(), 0.6f);
		CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 1.f);
		ShipRef->SetActorRotation(CurrentRotation);
		ShipRef->Root->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		ShipRef->Root->SetLinearDamping(0.2f);
		ForwardsSpeed = 500.f;
	}
	else
	{
		ShipRef->Root->SetLinearDamping(InitialLinearDamping);	
		ForwardsSpeed = 4000.f;
	}
}

void UHoveringMovementComponent::AddForce(FVector_NetQuantize End, int Num) const
{
	FVector ThrustForce, Start, CompLocation, UpVector = ShipRef->GetActorUpVector();
	float Constant = Gravity/4.f;//50000.f;
	float Distance;

	switch (Num)
	{
	case 1:
		Start = ShipRef->Thrust1->GetComponentLocation();
		CompLocation = ShipRef->Thrust1->GetComponentLocation();
		break;
	case 2:
		Start = ShipRef->Thrust2->GetComponentLocation();
		CompLocation = ShipRef->Thrust2->GetComponentLocation();
		break;
	case 3:
		Start = ShipRef->Thrust3->GetComponentLocation();
		CompLocation = ShipRef->Thrust3->GetComponentLocation();
		break;
	case 4:
		Start = ShipRef->Thrust4->GetComponentLocation();
		CompLocation = ShipRef->Thrust4->GetComponentLocation();
		break;
	default:
		Start = FVector::ZeroVector;
		break;
	}
	
	// Get distance from ShipRef->Thrust point to ground
	Distance = UKismetMathLibrary::Vector_Distance(Start, End) / ShipRef->TargetHeight;
	
	// Determine ShipRef->Thrust force based on distance
	ThrustForce = Constant * ShipRef->HoverForceCurve->GetFloatValue(Distance) * UpVector;

	// Add local downwards force if the ship is higher than it should be (TargetHeight)
	if (Distance > 1.f) { ShipRef->Root->AddForce(-UpVector * ShipRef->MinusHoverForceCurve->GetFloatValue(Distance)); }
	
	// Add the force at the specified location
	ShipRef->Root->AddForceAtLocation(ThrustForce, CompLocation);
}
