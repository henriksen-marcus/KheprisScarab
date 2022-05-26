// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostSpline.generated.h"

UCLASS()
class RACINGGAME_API AGhostSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGhostSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** Generate the spline from vectors and rotators. Arrays need to be ordered.
	 * @param Time The amount of time it took to complete the race.
	 */
	void GenerateSpline(const TArray<FVector>& Locations, const TArray<FRotator>& Rotations, const float& Time) const;
	
	UPROPERTY()
	class USplineComponent* Spline;
};
