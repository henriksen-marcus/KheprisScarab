// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerShipTesting.generated.h"

UCLASS()
class RACINGGAME_API APlayerShipTesting : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerShipTesting();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Root replacement */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* RtRpl;

	/** Ship main body */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* CustomCurve1;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* CustomCurve2;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier;

	/** What field of view the camera should interpolate towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetCameraFOV{90.f};

	/** Target spring arm length, constantly interpolated towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetSpringArmLength{2000.f};

	/** The ship's target height above the ground */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetHeight{1000.f};

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
	float CameraResetTime{1.f};
	

	/** Controls the forward movement of the root and cosmetic mesh rotation effect */
	void Forward(float Value);

	/** Controls the yaw rotation of the root and cosmetic mesh rotation effect */
	void Turn(float Value);
	
	/** Uses the MouseY input to change the relative pitch rotation of the spring arm */
	void CameraPitch(float Value);

	/** Uses the MouseX input to change the relative yaw rotation of the spring arm */
	void CameraYaw(float Value);

	void CameraZoomIn();
	void CameraZoomOut();

	/** Exits the game - Warning: If played in-editor, this will close the editor. */
	void EscPressed();

	
	/** Returns the rotation from the ships current to target rotation, where the target rotation is
	 * the cross-product of the vectors between the four raycast hit locations.
	 * Basically it gets the rotation that the object should have relative to the surface beneath. */
	FRotator GetSurfaceNormal();

	/** Returns the rotation from the ships current to target rotation, where the target rotation is
	 * the average between the normals of the four raycast hits.
	 * Basically it gets the rotation that the object should have relative to the surface beneath. */
	FRotator GetSurfaceNormalSimple() ;
	

	/** Interpolates towards the target using a custom curve. Returns the next position (not the delta) */
	float CustomInterp(float Current, float Target, float DeltaTime, float InterpSpeed = 4.f);

	float CustomInterp2(float Current, float Target, float DeltaTime, float InterpSpeed = 4.f);

	bool bPitchHasInput;
	bool bRollHasInput;

	float NextRollPosition;
	float NextPitchPosition;
	float NextYawPosition;

	/** Decides how much the root should move per tick */
	FVector LocalMove;

	float SpeedBoost;

	float CurrentYaw;
	bool bIsDashing;
	bool bIsJumping;

	float YawMove{};
	
	FRotator SpringArmRotTarget = FRotator::ZeroRotator;
	float FallSpeed{10.f};
	bool bLowThreshold = false;
	float CameraCenteringTimer{};

	FVector TargetLocation = FVector::ZeroVector;
	
	// Old code

	/** Returns the rotation of the cross-product of the vectors between the three raycast points
	 *	Basically it gets the rotation that the object should have relative to the surface beneath */
	//FRotator GetSurfaceNormal3P();
	
};
