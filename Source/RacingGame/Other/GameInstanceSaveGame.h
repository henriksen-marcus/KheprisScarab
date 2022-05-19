// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameInstanceSaveGame.generated.h"

/**
 * The SaveGame class for the game instance, aka. Global_Variables. Here lies information about what variables will be saves offline.
 */
UCLASS()
class RACINGGAME_API UGameInstanceSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UGameInstanceSaveGame();

	/** Shop Menu */
	UPROPERTY()
	float CurrentHealth{};
	
	UPROPERTY()
	float MaxHealth{};

	UPROPERTY()
	float MaxAmmo{};
	
	UPROPERTY()
	float CurrentAmmo{};

	UPROPERTY()
	float AmmoPercentage{};

	UPROPERTY()
	int Currency1{};
	
	UPROPERTY()
	int Currency2{};

	UPROPERTY()
	bool BoostPickup{};
	
	UPROPERTY()
	float MaxBoost{};

	UPROPERTY()
	int TimeCount{};
	
	UPROPERTY()
	int TimeAdded{};
	
	UPROPERTY()
	float DeltaTimeCount{};

	UPROPERTY()
	bool HealthActivate{};

	UPROPERTY()
	int HealthUpgradeTier{};

	UPROPERTY()
	int HealthUpgradeCost{};

	UPROPERTY()
	bool AmmoActivate{};

	UPROPERTY()
	int AmmoUpgradeTier{};
	
	UPROPERTY()
	float AmmoUpgradeCost{};

	UPROPERTY()
	bool BoostActivate{};

	UPROPERTY()
	int BoostUpgradeTier{};
	
	UPROPERTY()
	float BoostUpgradeCost{};

	UPROPERTY()
	bool TimeActivate{};

	UPROPERTY()
	int TimeUpgradeTier{};
	
	UPROPERTY()
	float TimeUpgradeCost{};


	/** Settings Menu */

	UPROPERTY()
	bool Health_Display{};

	UPROPERTY()
	bool Boost_Display{};

	UPROPERTY()
	bool Ammo_Display{};

	UPROPERTY()
	bool Time_Display{};

	UPROPERTY()
	bool Currency1_Display{};

	UPROPERTY()
	bool Currency2_Display{};

	UPROPERTY()
	bool Speed_Display{};

	UPROPERTY()
	bool RealTimer_Display{};

	UPROPERTY()
	bool Laps_Display{};

	UPROPERTY()
	bool Difficulty_Easy{};

	UPROPERTY()
	bool Difficulty_Normal{};

	UPROPERTY()
	bool Difficulty_Hard{};

	UPROPERTY()
	float GlobalVolumeMultiplier{};

	UPROPERTY()
	float HighScoreTime{};

	UPROPERTY()
	FLinearColor ShipColor{};

	UPROPERTY()
	bool bUseShipColor{};
	
	UPROPERTY()
	FDateTime SaveDate;
};
