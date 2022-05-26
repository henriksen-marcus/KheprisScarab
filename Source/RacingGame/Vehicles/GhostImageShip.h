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
	USkeletalMeshComponent* BaseMesh;
	
	/** Root replacement, using this for potential overlap events */
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* Root;

	/** Deprecated */
	UPROPERTY()
	TArray<FVector> LocationArr;

	/** Deprecated */
	UPROPERTY()
	TArray<FRotator> RotationArr;

	/** When the ghost image has finished it's playback, how long should it idle before despawning? */
	UPROPERTY(EditAnywhere)
	float DespawnTime{3.f};

	uint32 CurrentTick{};

	void MovementUpdate();

	void TargetPositionUpdate();

	void StartPlayback();

	void StopPlayback();

	void PausePlayback() const;

	void ResumePlayback() const;

	/** Spawns a spline actor that the ship follows. This is to create a smooth track that is frame-independent. */
	class AGhostSpline* SpawnSpline();

	FVector CurrentLocTarget;
	FRotator CurrentRotTarget;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* WidgetComp;

	bool bPlayback{};
	
	FTimerHandle PlayBackTimerHandle;

	UPROPERTY()
	class APlayerShipPhysics* PlayerRef;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	TArray<UMaterialInterface*> MatArr;

	float TickTimer{};

	UPROPERTY()
	class AGhostSpline* SplineRef;
};
