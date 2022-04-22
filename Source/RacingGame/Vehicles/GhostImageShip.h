// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostImageShip.generated.h"

UCLASS()
class RACINGGAME_API AGhostImageShip : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGhostImageShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;
	
	/** Root replacement */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* Root;
	

	UPROPERTY()
	TArray<FVector> LocationArr;

	UPROPERTY()
	TArray<FRotator> RotationArr;

	UPROPERTY()
	TArray<uint64> TickNumbers;

	bool bRaceStarted{};

	uint64 CurrentTick{};

	float IdleTimer{};

	void MovementUpdate();

};
