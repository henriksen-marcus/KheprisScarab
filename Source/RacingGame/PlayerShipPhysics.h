// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HoveringMovementComponent.h"
#include "PlayerShipPhysics.generated.h"

UCLASS()
class RACINGGAME_API APlayerShipPhysics : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerShipPhysics();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraVariables")
	UCameraComponent* Camera;
	
private:
	/** Root replacement */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* Root;

	/** Ship main body */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* CustomCurve1;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* CustomCurve2;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* JumpCurve;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* HoverForceCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	int MaxAmmo{30};

	/** How long the dash lasts */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float DashTimer{2.f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost{1.5f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier{1.f};

	/** What field of view the camera should interpolate towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetCameraFOV{90.f};

	/** Target spring arm length, constantly interpolated towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetSpringArmLength{2000.f};

	/** The ship's target height above the ground */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetHeight{1000.f};

	/** Whether debug vector math lines should be drawn */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bEnableDebugLines{true};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bLogSpeed{false};

	/** This controls the engine running sound */
	UPROPERTY(EditAnywhere, Category = "Sound")
	UAudioComponent* AudioComp;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* StartSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* BoostSound;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust1;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust2;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust3;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust4;

	/** Seconds of inactivity needed for the spring arm to reset its rotation */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float CameraResetTime{1.2f};

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	TSubclassOf<UCameraShakeBase> CamShake;

	UPROPERTY()
	UHoveringMovementComponent* MoveComp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	// ---------- Functions ---------- //
	
	
	/** Controls the forward movement of the root and cosmetic mesh rotation effect */
	void Forward(float Value);

	/** Controls the yaw rotation of the root and cosmetic mesh rotation effect */
	void Turn(float Value);
	
	/** Uses the MouseY input to change the relative pitch rotation of the spring arm */
	void CameraPitch(float Value);

	/** Uses the MouseX input to change the relative yaw rotation of the spring arm */
	void CameraYaw(float Value);

	void Dash();
	void Jump();

	void Crouch();
	void CrouchEnd();

	void CameraZoomIn();
	void CameraZoomOut();
	
	/** Returns the rotation from the ships current to target rotation, where the target rotation is
	 * the cross-product of the vectors between the four raycast hit locations.
	 * Basically it gets the rotation that the object should have relative to the surface beneath. */
	FRotator GetSurfaceNormal();
	
	// ---------- Variables ---------- //
	
	bool bPitchHasInput{};
	bool bRollHasInput{};

	float NextRollPosition{};
	float NextPitchPosition{};
	float NextYawPosition{};

	/** Decides how much the root should move additionally, per tick */
	FVector LocalMove = FVector::ZeroVector;

	float SpeedBoost{1.f};
	
	bool bIsDashing{};
	bool bIsJumping{};

	/** How much the players local yaw should change per tick */
	float YawMove{};
	
	FRotator SpringArmRotTarget = FRotator::ZeroRotator;
	
	bool bLowThreshold = false;
	float CameraCenteringTimer{};

	/** Increases the longer the player in in the air, decides how fast the Z location decreases */
	float FallSpeed{1.f};

	/** Decides what curve value the FallSpeed interpolation should be at */
	float FallTimer{};

	/** Number that the engine sound pitch is multiplied with */
	float PitchMultiplier{1.f};

	float JumpTimer{};
	
	FVector TargetLocation = FVector::ZeroVector;
	FVector InitialLocation;
	float InitialTargetHeight{};
	float LocationInterpolationSpeed{10.f};
	float DistanceFromGround{1000.f};
	
	FVector Force = FVector::ZeroVector;
	FVector RForce = FVector::ZeroVector;
	
	float ShipWeight{};

	TArray<float>ForceArray;
};
