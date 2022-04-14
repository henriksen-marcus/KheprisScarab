// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HoveringMovementComponent.generated.h"

/**
 * Movement component to control and execute the movement of a ship.
 * Code comes from the original PlayerShipPhysics.cpp
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RACINGGAME_API UHoveringMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHoveringMovementComponent();

	/** Reference to owning class */
	UPROPERTY(VisibleAnywhere)
	class APlayerShipPhysics* ShipRef;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	/** Controls the forward movement of the root and cosmetic mesh rotation effect */
	void Forward(float Value);

	/** Controls the yaw rotation of the root and cosmetic mesh rotation effect */
	void Turn(float Value);

	void MovementUpdate();

	/**
	 *  Adds force at one of the 4 thrust locations.
	 *  @param End			Hit location of the raycast.
	 *  @param Num			Which of the four thrust locations to apply the thrust force.
	 */
	void AddForce(FVector_NetQuantize End, int Num) const;


	
	float JumpTimer{};
	
	/** The location the player spawns at, aka the position of the player start */
	FVector InitialLocation;
	
	float InitialTargetHeight{};
	float DistanceFromGround{500.f};
	float Gravity{};

	/** Well, it's... the ships weight. */
	float ShipWeight{};

	float InitialLinearDamping{};

	float ForwardsSpeed{4000.f};
	float ActualSpeed{};

	/** The amount of force to be applied the next tick */
	FVector Force = FVector::ZeroVector;

	float NextRollPosition{};
	float NextPitchPosition{};
	float NextYawPosition{};

	/** The actual value that is multiplied with the speed. Will copy MaxSpeedBoost when Dash() is used. */
	float SpeedBoost{1.f};
	
	bool bIsDashing{};
	bool bIsJumping{};

	/** How much the players local yaw should change per tick */
	float YawMove{};

	bool bEnableDebugLines = true;
};
