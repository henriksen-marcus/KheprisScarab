// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HoveringMovementComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "Engine/AutoDestroySubsystem.h"
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
	USkeletalMeshComponent* BaseMesh;
	
	/** Root replacement */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* Root;

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UCapsuleComponent* Collision;

	
	/** CAMERA */

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* BackCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* BehindCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* FrontCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USceneCaptureComponent2D* BirdCam;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY()
	UCameraComponent* ActiveCamera;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* BackSpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* BehindSpringArm;

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

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost{1.4f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier{1.f};

	/** What field of view the camera should interpolate towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetCameraFOV{105.f};

	/** Target spring arm length, constantly interpolated towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetSpringArmLength{700.f};

	/** The ship's target height above the ground */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetHeight{300.f};

	/** Seconds of inactivity needed for the spring arm to reset its rotation */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float CameraResetTime{1.5f};

	/** Changes how powerful the gravity affecting the ship is */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float GravityScalar{1.f};

	/** Whether debug vector math lines should be drawn */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bEnableDebugLines{false};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool bLogSpeed{false};
	
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MinimapDistance{100000.f};

	UPROPERTY(BlueprintReadOnly, Category = "BlueprintVariables")
	float Speed{};
	
	UPROPERTY(BlueprintReadOnly, Category = "BlueprintVariables")
    bool bIsBraking{};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	TSubclassOf<class ABullet> BulletClassToSpawn;


	/* AUDIO */
	
	/** This controls the engine running sound */
	UPROPERTY(EditAnywhere, Category = "Sound")
	UAudioComponent* AudioComp;

	/** Engine start sound */
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* StartSound;

	/** Speed boost sound */
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* BoostSound;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* GunClickSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound1;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HitSound2;
	
	
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


	/** OTHER */

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NS_SandSystem;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NS_SandSystem_End;

	UPROPERTY()
	UNiagaraComponent* SandSystemPtr;

	UPROPERTY()
	UNiagaraComponent* SandSystemEndPtr;

	/** The widget blueprint that will pop up when the player drives off road. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> OffTrackScreenClass;

	UPROPERTY()
	UUserWidget* OffTrackScreen;

	/** Whether the player is on the road or not. */
	UPROPERTY(BlueprintReadOnly)
	bool bIsOnRoad{};
	

	// ---------- FUNCTIONS ---------- //
	
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

	void Shoot();
	
	void Dash();
	void Jump();
	
	void Respawn();

	void Crouch();
	void CrouchEnd();

	/** Cycles between 3 camera angles */
	void ChangeCameraAngle();

	/** Switch between front and back camera/look behind */
	void LookBehind();

	void MovementUpdate();
	void HoverRaycast();

	/**
	 *  Adds force at one of the 4 thrust locations.
	 *  @param End			Hit location of the raycast.
	 *  @param Num			Which of the four thrust locations to apply the thrust force.
	 */
	void AddForce(FVector_NetQuantize End, int Num) const;

	void RaycastHover();

	void CameraUpdate();

	/**	Returns the name of the physics material on the ground beneath the ship. */
	FString CheckSurface(FVector &HitLocation);

	/** Spawns a niagara sand system effect at the specified location. If a system already exists, update position. */
	void SpawnSandEffect(FVector HitLoc);

	/** Spawns a niagara sand system vanishing effect at the specified location. If a system already exists, update position. */
	void SpawnSandEffectEnd(FVector HitLoc);

	
	// ---------- Variables ---------- //

	enum ECameraAngle : uint8
	{
		Close,
		Far,
		Front,
		END_ENUM
	};

	int8 CurrentCameraAngle{};

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
	FRotator InitialBehindSpringArmRotation = FRotator::ZeroRotator;

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

	float ForwardsSpeed{5500.f};
	
	float HitSoundCooldown{};
	bool bEnableDrag{true};

	FString CurrentSurface{};

	UPROPERTY()
	class UGlobal_Variables* GameInstance;

	UPROPERTY()
	class ARacingGameGameModeBase* GamemodeBase;
	
	/** Timers */
	float ShootTimer{};
	float JumpTimer{};
	float OffTrackTimer{};
	float SandEndSystemTimer{};

	/** Bools */
};
