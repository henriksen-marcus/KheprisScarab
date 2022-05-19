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

	//Variables that needs to be saved
	
	#pragma region Shop_Menu
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float CurrentHealth{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float MaxHealth{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float MaxAmmo{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float CurrentAmmo{};

	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float AmmoPercentage{};

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
		int TimeStartCount{60};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		int TimeAdded{};
	UPROPERTY(EditAnywhere, Category = "Upgradeable Variables")
		float DeltaTimeCount{};

	bool HealthActivate{};
	int HealthUpgradeTier{};
	int HealthUpgradeCost{ 2 };

	bool AmmoActivate{};
	int AmmoUpgradeTier{};
	float AmmoUpgradeCost{ 2 };

	bool BoostActivate{};
	int BoostUpgradeTier{};
	float BoostUpgradeCost{ 2 };

	bool TimeActivate{};
	int TimeUpgradeTier{};
	float TimeUpgradeCost{ 2 };
	#pragma endregion

	#pragma region Settings_Menu
	bool Health_Display{ true };
	bool Boost_Display{ true };
	bool Ammo_Display{ true };
	bool Time_Display{ true };
	bool Currency1_Display{ true };
	bool Currency2_Display{ true };
	bool Speed_Display{ true };
	bool RealTimer_Display{ true };
	bool Laps_Display{ true };

	bool Difficulty_Easy{ false };
	bool Difficulty_Normal{ true };
	bool Difficulty_Hard{ false };


	UPROPERTY(BlueprintReadWrite)
	float GlobalVolumeMultiplier{ 1.f };
	#pragma endregion

	#pragma region High Score
	float HighScoreTime{};
	#pragma endregion


	//---------------------------------------


	//other variables
	float DashTimer{3.5f};
	
	#pragma region Modes
	UPROPERTY(BlueprintReadWrite, Category = "Variable")
		bool Racing{};


	UPROPERTY(BlueprintReadOnly)
	bool TimeAttackMode{};
	#pragma endregion

	#pragma region CheckPoints
	bool CheckPoint_Connected{ false };
	bool NewCheckPoint{ false };
	int PlayerCheckpointNumber{};

	UPROPERTY(BlueprintReadOnly)
	int CurrentLap_Counter{ 1 };
	
	int MaxLap_Counter{ 3 };
	#pragma endregion

	#pragma region StartRace
	bool bRaceNotStarted{};

	UPROPERTY(BlueprintReadWrite)
		bool Pause{};
	#pragma endregion

	#pragma region Music
	bool MainMenu_Music{};
	#pragma endregion

	#pragma region Timer
	float InGame_Timer{};
	#pragma endregion

	enum ESaveType : uint8
	{
		GameState,
		GhostImage
	};


	UPROPERTY()
	class APlayerShipPhysics* PlayerRef;

	/** Change the health of the player by a certain delta. Clamps values and checks for zero. */
	void AddHealth(const float Amount);

	UFUNCTION(BlueprintCallable)
	void ChangeLevel();
	
	bool LoadGameState();

	/** Blends the ships texture with the given color. This function also works as a color-enabler. */
	UFUNCTION(BlueprintCallable)
	void ChangeColor(FLinearColor NewColor);

	/** Removes the color blend on the ships texture, revealing the original texture. */
	UFUNCTION(BlueprintCallable)
	void RemoveColor();

	bool SaveGame(ESaveType SaveType);

	

	enum EGhostDifficulty : uint8
	{
		Player,
		Gold,
		Silver,
		Bronze
	};
	
	EGhostDifficulty GhostDifficulty{};

	UPROPERTY(BlueprintReadOnly)
	int32 GhostDiffBP{};

	void SetDifficulty(EGhostDifficulty Difficulty);

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* PauseScreenRef;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor ShipColor;

	UPROPERTY(BlueprintReadWrite)
	bool bUseShipColor{};

	bool bIsGameLoaded{};

	//----------------------------------------


	//Fjernes
	bool Sound{ true };
	float Music_Volum{};
};
