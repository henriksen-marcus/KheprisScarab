// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Global_Variables.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UGlobal_Variables : public UGameInstance
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	UGlobal_Variables();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay();

public:
	// Called every frame
	virtual void Tick(float DeltaTime);
	
public:
	#pragma region Modes
	UPROPERTY(BlueprintReadWrite, Category = "Variable")
	bool Racing{};
	#pragma endregion

	#pragma region Modes
	bool TimeAttackMode{};
	#pragma endregion


	#pragma region Shop_Menu
	float DashTimer{2.f};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float CurrentHealth{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float MaxHealth{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float MaxAmmo{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float CurrentAmmo{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float AmmoPrecentage{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		int Currency1{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		int Currency2{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		bool BoostPickup{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float MaxBoost{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		int TimeCount{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		int TimeAdded{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float DeltaTimeCount{};

	bool HealthActivate{};
	int HealthUpgradeTier{};
	int HealthUpgradeCost{2};

	bool AmmoActivate{};
	int AmmoUpgradeTier{};
	float AmmoUpgradeCost{2};

	bool BoostActivate{};
	int BoostUpgradeTier{};
	float BoostUpgradeCost{2};

	bool TimeActivate{};
	int TimeUpgradeTier{};
	float TimeUpgradeCost{2};
	#pragma endregion

	#pragma region Settings_Menu
	bool Health_Display{true};
	bool Boost_Display{true};
	bool Ammo_Display{true};
	bool Time_Display{true};
	bool Currency1_Display{true};
	bool Currency2_Display{true};
	bool Speed_Display{ true };

	bool Difficulty_Easy{false};
	bool Difficulty_Normal{true};
	bool Difficulty_Hard{false};

	bool Sound{true};
	#pragma endregion

	#pragma region CheckPoints
	int PlayerCheckpointNumber{};
	#pragma endregion

	#pragma region StartRace
	bool RaceStartOFF{};
	#pragma endregion

#pragma region Music
	bool MainMenu_Music{};

	float Music_Volum{};
#pragma endregion	

};
