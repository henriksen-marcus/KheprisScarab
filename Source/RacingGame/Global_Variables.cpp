// Fill out your copyright notice in the Description page of Project Settings.


#include "Global_Variables.h"
#include "Vehicles/PlayerShipPhysics.h"
#include "Other/GameInstanceSaveGame.h"
#include "Other/GhostImageSaveGame.h"
#include "RacingGameGameModeBase.h"

UGlobal_Variables::UGlobal_Variables()
{
	Currency1 = 0;
	Currency2 = 0;

	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
	TimeAdded = 15;

	for (size_t i = 0; i < 100; i++)
	{
		PlayerCheckpointTime_Array.Add(false);
		GhostCheckpointTime.Add(false);
	}
}

void UGlobal_Variables::BeginPlay()
{
}

void UGlobal_Variables::Tick(float DeltaTime)
{
	if (Sound == true)
		Music_Volum = 1;
	else
		Music_Volum = 0;
}


void UGlobal_Variables::AddHealth(const float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);

	/*if (!CurrentHealth)
	{
		if (PlayerRef)
		{
			PlayerRef->Respawn();
			// Handle respawn screens and effects here, or do it in playershipphysics
			CurrentHealth = MaxHealth;
		}
	}*/
}

void UGlobal_Variables::ChangeLevel()
{
	//UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
	/**
	 * The point of latent actions are that they can take more than one frame to execute and then will trigger a function when finished.
	 * Fill out the struct:
	 */

	/*UE_LOG(LogTemp, Warning, TEXT("Starting level load!"))
	
	FLatentActionInfo ActionInfo;
	ActionInfo.CallbackTarget = this;
	ActionInfo.ExecutionFunction = FName("MapLoadCompleted");
	ActionInfo.Linkage = 0;
	ActionInfo.UUID = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Master_Map"), true, true, ActionInfo);*/
	
}

bool UGlobal_Variables::SaveGame(ESaveType SaveType)
{
	UE_LOG(LogTemp, Warning, TEXT("SaveGame(), mvh. gameinstance"))
	switch (SaveType)
	{
	case GameState:
		{
			UGameInstanceSaveGame* SGI = Cast<UGameInstanceSaveGame>(UGameplayStatics::CreateSaveGameObject(UGameInstanceSaveGame::StaticClass()));

			if (!SGI) { return false; }
		
			// Loading information into save object
			SGI->CurrentHealth = CurrentHealth;
			SGI->MaxHealth = MaxHealth;
			SGI->MaxAmmo = MaxAmmo;
			SGI->CurrentAmmo = CurrentAmmo;
			SGI->AmmoPercentage = AmmoPercentage;
			SGI->Currency1 = Currency1;
			SGI->Currency2 = Currency2;
			SGI->BoostPickup = BoostPickup;
			SGI->MaxBoost = MaxBoost;
			SGI->TimeCount = TimeCount;
			SGI->TimeAdded = TimeAdded;
			SGI->DeltaTimeCount = DeltaTimeCount;
			SGI->HealthActivate = HealthActivate;
			SGI->HealthUpgradeTier = HealthUpgradeTier;
			SGI->HealthUpgradeCost = HealthUpgradeCost;
			SGI->AmmoActivate = AmmoActivate;
			SGI->AmmoUpgradeTier = AmmoUpgradeTier;
			SGI->AmmoUpgradeCost = AmmoUpgradeCost;
			SGI->BoostActivate = BoostActivate;
			SGI->BoostUpgradeTier = BoostUpgradeTier;
			SGI->BoostUpgradeCost = BoostUpgradeCost;
			SGI->TimeActivate = TimeActivate;
			SGI->TimeUpgradeTier = TimeUpgradeTier;
			SGI->TimeUpgradeCost = TimeUpgradeCost;
			SGI->Health_Display = Health_Display;
			SGI->Boost_Display = Boost_Display;
			SGI->Ammo_Display = Ammo_Display;
			SGI->Time_Display = Time_Display;
			SGI->Currency1_Display = Currency1_Display;
			SGI->Currency2_Display = Currency2_Display;
			SGI->Speed_Display = Speed_Display;
			SGI->RealTimer_Display = RealTimer_Display;
			SGI->Laps_Display = Laps_Display;
			SGI->Difficulty_Easy = Difficulty_Easy;
			SGI->Difficulty_Normal = Difficulty_Normal;
			SGI->Difficulty_Hard = Difficulty_Hard;
			SGI->GlobalVolumeMultiplier = GlobalVolumeMultiplier;
			SGI->HighScoreTime = HighScoreTime;
			SGI->ShipColor = ShipColor;
			SGI->bUseShipColor = bUseShipColor;
			SGI->TimeAddedFromShop = TimeAddedFromShop;
			SGI->TimeStartCount = TimeStartCount;
			
			if (UGameplayStatics::SaveGameToSlot(SGI, TEXT("GameState"), 0))
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully saved game state."))
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Was not able to save game state."))
				return false;
			}
		}
		break;
	case GhostImage:
		{
			// Save the ghost image
			//Create an instance of the save-game class
			UGhostImageSaveGame* SaveGameInstance;
			SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::CreateSaveGameObject(UGhostImageSaveGame::StaticClass()));

			ARacingGameGameModeBase* GameModeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());
			if (GameModeBase)
			{
				UE_LOG(LogTemp, Warning, TEXT("Saving game... Location Arr size: %d"), GameModeBase->LocationArr.Num())
				SaveGameInstance->LocationArr = GameModeBase->LocationArr;
				SaveGameInstance->RotationArr = GameModeBase->RotationArr;
				UE_LOG(LogTemp, Warning, TEXT("Saved game... Location Arr size in saveinstance: %d"), SaveGameInstance->LocationArr.Num())
			}
			
			if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerGhost"), 0))
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully saved the Bronze ghost image. Recording size: %d"), GameModeBase->LocationArr.Num())
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Was not able to save ghost image."))
				return false;
			}
		}
		break;
	default:
		return false;
	}
	
	return false;
}

void UGlobal_Variables::SetDifficulty(int32 Difficulty)
{
	GhostDifficulty = Difficulty;
	GhostDiffBP = Difficulty;
}

bool UGlobal_Variables::LoadGameState()
{
	// Load the entire game
	UE_LOG(LogTemp, Warning, TEXT("loading game"))
	if (!UGameplayStatics::DoesSaveGameExist(TEXT("GameState"), 0)) { return false; }
	
	UGameInstanceSaveGame* SGI = Cast<UGameInstanceSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GameState"), 0));

	if (!SGI) { return false; }

	CurrentHealth=SGI->CurrentHealth;
	MaxHealth= SGI->MaxHealth;
	MaxAmmo= SGI->MaxAmmo;
	CurrentAmmo= SGI->CurrentAmmo;
	AmmoPercentage= SGI->AmmoPercentage;
	Currency1= SGI->Currency1;
	Currency2= SGI->Currency2;
	BoostPickup= SGI->BoostPickup;
	MaxBoost= SGI->MaxBoost;
	TimeCount= SGI->TimeCount;
	TimeAdded= SGI->TimeAdded;
	DeltaTimeCount= SGI->DeltaTimeCount;
	HealthActivate= SGI->HealthActivate;
	HealthUpgradeTier= SGI->HealthUpgradeTier;
	HealthUpgradeCost= SGI->HealthUpgradeCost;
	AmmoActivate= SGI->AmmoActivate;
	AmmoUpgradeTier= SGI->AmmoUpgradeTier;
	AmmoUpgradeCost= SGI->AmmoUpgradeCost;
	BoostActivate= SGI->BoostActivate;
	BoostUpgradeTier= SGI->BoostUpgradeTier;
	BoostUpgradeCost= SGI->BoostUpgradeCost;
	TimeActivate= SGI->TimeActivate;
	TimeUpgradeTier= SGI->TimeUpgradeTier;
	TimeUpgradeCost= SGI->TimeUpgradeCost;
	Health_Display= SGI->Health_Display;
	Boost_Display= SGI->Boost_Display;
	Ammo_Display= SGI->Ammo_Display;
	Time_Display= SGI->Time_Display;
	Currency1_Display= SGI->Currency1_Display;
	Currency2_Display= SGI->Currency2_Display;
	Speed_Display= SGI->Speed_Display;
	RealTimer_Display= SGI->RealTimer_Display;
	Laps_Display= SGI->Laps_Display;
	Difficulty_Easy= SGI->Difficulty_Easy;
	Difficulty_Normal= SGI->Difficulty_Normal;
	Difficulty_Hard= SGI->Difficulty_Hard;
	GlobalVolumeMultiplier= SGI->GlobalVolumeMultiplier;
	ShipColor = SGI->ShipColor;
	bUseShipColor = SGI->bUseShipColor;
	HighScoreTime = SGI->HighScoreTime;
	TimeAddedFromShop = SGI->TimeAddedFromShop;
	TimeStartCount = SGI->TimeStartCount;
	
	return true;
}

void UGlobal_Variables::ChangeColor(FLinearColor NewColor)
{
	ShipColor = NewColor;
	bUseShipColor = true;
}

void UGlobal_Variables::RemoveColor() 
{
	ShipColor.A = 0.f;
	bUseShipColor = false;
}




