// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HoveringMovementComponent.h"
#include "RacingGame/Other/Bullet.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
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


	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;
	
	/** Root replacement */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* Root;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* BackCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* FrontCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* BackSpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* FrontSpringArm;

	UPROPERTY()
	USpringArmComponent* ActiveSpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	TSubclassOf<UCameraShakeBase> CamShake;


	/* CURVES */
	
	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* CustomCurve1;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* CustomCurve2;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* HoverForceCurve;

	UPROPERTY(EditAnywhere, Category = "Curves")
	UCurveFloat* MinusHoverForceCurve;
	

	
	/* BLUEPRINT VARIABLES */
	
	/** How long the dash lasts */
	//UPROPERTY(EditAnywhere, Category = "EditableVariables")
	//float DashTimer{2.f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost{1.5f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier{1.f};

	/** What field of view the camera should interpolate towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetCameraFOV{90.f};

	/** Target spring arm length, constantly interpolated towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetSpringArmLength{600.f};

	/** The ship's target height above the ground */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetHeight{800.f};

	/** Seconds of inactivity needed for the spring arm to reset its rotation */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float CameraResetTime{1.2f};

	/** Changes how powerful the gravity affecting the ship is */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float GravityScalar{1.f};

	/** Whether debug vector math lines should be drawn */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bEnableDebugLines{true};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bLogSpeed{false};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	TSubclassOf<class ABullet> BulletClassToSpawn;


	/* AUDIO */
	
	/** This controls the engine running sound */
	UPROPERTY(EditAnywhere, Category = "Sound")
	UAudioComponent* AudioComp;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* StartSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* BoostSound;

	// Reload functionality is discontinued due to pickups.
	/*UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadSound;*/

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound1;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound2;

	/*UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound3;*/
	
	/* LOCATION PLACEHOLDERS */

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust1;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust2;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust3;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust4;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* BulletSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Movement")
	UHoveringMovementComponent* MovementComponent;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NS_SandSystem;

	UPROPERTY(EditAnywhere)
	UMaterial* MyMat;

	UPROPERTY()
	UNiagaraComponent* SandSystemPtr;
	

	// ---------- Functions ---------- //
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	
	/** Controls the forward movement of the root and cosmetic mesh rotation effect */
	void Forward(float Value);

	/** Controls the yaw rotation of the root and cosmetic mesh rotation effect */
	void Turn(float Value);
	
	/** Uses the MouseY input to change the relative pitch rotation of the spring arm */
	void CameraPitch(float Value);

	/** Uses the MouseX input to change the relative yaw rotation of the spring arm */
	void CameraYaw(float Value);

	void Shoot(float Value);

	//void Reload();
	void Dash();
	void Jump();
	
	void Respawn();

	void Crouch();
	void CrouchEnd();

	void CameraZoomIn();
	void CameraZoomOut();

	UFUNCTION(BlueprintCallable)
	/** Set the world time dilation (how fast time goes by) for a specified duration.
	 *	@param Amount		New time dilation, default is 1.
	 *	@param Duration		How long the dilation should last.
	 */
	void SloMo(float Amount, float Duration);

	/** Switch between front and back camera/look behind */
	void CameraSwap();

	void MovementUpdate();

	/**
	 *  Adds force at one of the 4 thrust locations.
	 *  @param End			Hit location of the raycast.
	 *  @param Num			Which of the four thrust locations to apply the thrust force.
	 *  @param bHit			If the raycast returned a hit.
	 */
	void AddForce(FVector_NetQuantize End, int Num, bool bHit) const;

	void RaycastHover();

	void CameraUpdate();

	/**	Returns the name of the physics material on the ground beneath the ship. */
	FString CheckSurface(FVector &HitLocation);

	/** Spawns a niagara sand system effect at the surface location under the ship, if the physical material is right. */
	void SpawnSandEffect();

	
	// ---------- Variables ---------- //

	FTimerHandle TimeDilationHandle;
	
	
	bool bForwardHasInput{};
	bool bRollHasInput{};

	float NextRollPosition{};
	float NextPitchPosition{};
	float NextYawPosition{};

	/** The actual value that is multiplied with the speed. Will copy MaxSpeedBoost when Dash() is used. */
	float SpeedBoost{1.f};
	
	bool bIsDashing{};
	bool bIsJumping{};
	bool bIsReloading{};

	/** How much the players local yaw should change per tick */
	float YawMove{};
	
	FRotator SpringArmRotTarget = FRotator::ZeroRotator;
	FRotator SpringArmLocalChange = FRotator::ZeroRotator;
	FRotator InitialBackSpringArmRotation = FRotator::ZeroRotator;
	FRotator InitialFrontSpringArmRotation = FRotator::ZeroRotator;

	/** Takes the time for how long the camera movement has been idle */
	float CameraCenteringTimer{};
	
	/** Number that the engine sound pitch is multiplied with */
	float PitchMultiplier{1.f};

	/** The location the player spawns at, aka the position of the player start */
	FVector InitialLocation;
	
	float InitialTargetHeight{};
	float DistanceFromGround{500.f};
	float Gravity{};

	/** The amount of force to be applied the next tick */
	FVector Force = FVector::ZeroVector;

	/** Well, it's... the ships weight. */
	float ShipWeight{};

	float InitialLinearDamping{};

	float ForwardsSpeed{5500.f};
	float ActualSpeed{};
	float ShootTimer{};
	float JumpTimer{};
	float HitSoundCooldown{};
	float LinearDampingReduction{1.f};
	bool bEnableDrag{true};

	UPROPERTY()
	class UGlobal_Variables* GameInstance;
	
};
