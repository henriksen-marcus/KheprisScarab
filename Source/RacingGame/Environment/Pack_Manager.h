// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pack_Manager.generated.h"

UCLASS()
class RACINGGAME_API APack_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APack_Manager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, Category = "Collision")
		class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;
	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMesh* EmptyMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Material")
		class UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* PackSound_OnPlayerHit;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* PackSound_Default;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void Movement(float DeltaTime);

public:
	float BounceSpeed;
	float BounceHight;
	float RotationSpeed;

	float RunningTime;
	float WaitingTime;

	FVector Velocity;
	FRotator Rotation;

	//Variables when changed in Blueprint, changes the packs' function on collision
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool HealthPack;
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool AmmoPack;
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool BoostPack;
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool Currency1;
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool Currency2;
	UPROPERTY(EditAnywhere, Category = "Editable Variable")
		bool TimePack;

	//Collision Sound
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* HealthPack_Sound;
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* AmmoPack_Sound;
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* BoostPack_Sound;
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* Currency1_Sound;
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* Currency2_Sound;
	UPROPERTY(EditAnywhere, Category = "Collision Sound")
		USoundBase* Time_Sound;

};
