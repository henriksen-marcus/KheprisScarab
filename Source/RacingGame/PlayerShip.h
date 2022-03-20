// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "PlayerShip.generated.h"

UCLASS()
class RACINGGAME_API APlayerShip : public APawn
{
	GENERATED_BODY()
public:
	APlayerShip();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* RtRpl; // Root replacement

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* DistanceCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* JumpCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* LocalMoveCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	int MaxAmmo;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float DashTimer{2.f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetCameraFOV{90.f};

	/** Target spring arm length, constantly interpolated towards */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TargetSpringArmLength{2000.f};
	
	//UPROPERTY()
	//TArray<UArrowComponent*> ThrustLocations;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust1;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust2;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust3;

	UPROPERTY(EditAnywhere, Category = "Arrows")
	UArrowComponent* Thrust4;

	/** Seconds of inactivity needed for camera to reset its rotation */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float CameraResetTime{1.f};

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector InitialLocation;

	/** Controls the forward movement of the root and cosmetic mesh rotation effect */
	void Forward(float Value);

	/** Controls the yaw rotation of the root and cosmetic mesh rotation effect */
	void Turn(float Value);
	
	/** Uses the MouseY input to change the relative pitch rotation of the spring arm */
	void CameraPitch(float Value);

	/** Uses the MouseX input to change the relative yaw rotation of the spring arm */
	void CameraYaw(float Value);

	void Dash();
	void ResetDash();
	
	void Jump();
	void JumpEnd();

	/** Exits the game - Warning: If played in-editor, this will close the editor. */
	void EscPressed();

	/** Returns the rotation of the cross-product of the vectors between the three raycast points
	 *	Basically it gets the rotation that the object should have relative to the surface beneath */
	FRotator GetSurfaceNormal3P();

	/** Returns the rotation of the cross-product of the vectors between the four raycast points
	 * Basically it gets the rotation that the object should have relative to the surface beneath */
	FRotator GetSurfaceNormal4P();

	/** Returns the Z height the object should have to stay a predetermined height above the ground */
	float GetTargetZ();

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
	FVector TempVec = FVector::ZeroVector;
	float FallTime{};
	bool bLowThreshold = false;
	float CameraCenteringTimer{};
	
	
};
