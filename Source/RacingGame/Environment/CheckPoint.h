// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPoint.generated.h"

UCLASS()
class RACINGGAME_API ACheckPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Checkpoint Number")
		int ThisCheckpointNumber{};
	
	UPROPERTY(EditAnywhere, Category = "Collision")
		class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Material")
		class UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere, Category = "Arrow")
		class UArrowComponent* SpawnPointArrow;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere, Category = "Checkpoint")
		bool bIsFirst_CheckPoint{};
	UPROPERTY(EditAnywhere, Category = "Checkpoint")
		bool bIsGoal{};
};
