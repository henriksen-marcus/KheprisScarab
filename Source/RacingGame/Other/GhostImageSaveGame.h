// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GhostImageSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UGhostImageSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UGhostImageSaveGame();

	UPROPERTY(EditAnywhere)
	FString SaveName;
	
	UPROPERTY(EditAnywhere)
	FString DateTime;

	UPROPERTY(EditAnywhere)
	FVector EndLocation;
	
	UPROPERTY(EditAnywhere)
	TArray<FVector> LocationArr;

	UPROPERTY(EditAnywhere)
	TArray<FRotator> RotationArr;

	UPROPERTY(EditAnywhere)
	TArray<uint64> TickNumbers;
};
