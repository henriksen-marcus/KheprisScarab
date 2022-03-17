// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
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
		float DashTimer;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
		UMaterialInterface* DamageMaterial;

	UPROPERTY(VisibleAnywhere)
		TArray<UArrowComponent*> ThrustLocations;


	FVector InitialLocation;

	/** Player input */
	void Roll(float Value);
	void Pitch(float Value);

	void CameraPitch(float Value);
	void Yaw(float Value);

	void Dash();
	void ResetDash();
	void Jump();
	void JumpEnd();
	void EscPressed();

	bool bPitchHasInput;
	bool bRollHasInput;

	float NextRollPosition;
	float NextPitchPosition;
	float NextYawPosition;

	float TargetZ{};

	float SpeedBoost;

	float CurrentYaw;
	float InitialArmLength;

	bool bIsDashing;

	bool bIsJumping;
	bool IgnoreInput;
	bool temp = false;
	float ForceChange = 1.f;

	FVector Force = FVector::ZeroVector;

};
