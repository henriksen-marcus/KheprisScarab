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

	UPROPERTY() // All these variables need to be UPROPERTY!
	FString SaveName;

	UPROPERTY()
	FString DateTime;

	/** The time it took to complete the race. */
	UPROPERTY()
	float Time{};

	// Information is stored at 30Hz/30 FPS.
	UPROPERTY()
	TArray<FVector> LocationArr;

	UPROPERTY()
	TArray<FRotator> RotationArr;
};
