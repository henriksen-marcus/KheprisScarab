// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pack_Heath.generated.h"

UCLASS()
class RACINGGAME_API APack_Heath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APack_Heath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void Movement(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
		class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* Pack_Health_HitSound;

public:
	float BounceSpeed;
	float BounceHight;
	float RotationSpeed;

	float RunningTime;
	float WaitingTime;
};
