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
	int MaxAmmo;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float DashTimer;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpeedMultiplier;
	
	UPROPERTY(VisibleAnywhere)
	TArray<UArrowComponent*> ThrustLocations;

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
	FRotator GetSurfaceNormal();

	bool bPitchHasInput;
	bool bRollHasInput;

	float NextRollPosition;
	float NextPitchPosition;
	float NextYawPosition;

	/** Decides how much the root should move per tick */
	FVector LocalMove;

	/***/
	FRotator NewRotation = FRotator::ZeroRotator;
	
	float TargetZ{};

	float SpeedBoost;

	float CurrentYaw;
	float InitialArmLength;
	bool bIsDashing;
	bool bIsJumping;

	float YawMove{};
	
	FRotator CameraMove = FRotator::ZeroRotator;
	
};
