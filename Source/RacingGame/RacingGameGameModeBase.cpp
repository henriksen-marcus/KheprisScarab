// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingGameGameModeBase.h"
#include "Other/GhostImageSaveGame.h"
#include "Vehicles/GhostImageShip.h"
#include "Vehicles/PlayerShipPhysics.h"
#include "Other/GameInstanceSaveGame.h"

ARacingGameGameModeBase::ARacingGameGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ARacingGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	APawn* Ref = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (Ref)
	{
		PlayerShipRef = Cast<APlayerShipPhysics>(Ref);
	}

	GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	
	LoadGameState();

	// Ghost
	if (GameInstance)
	{
		if (GameInstance->TimeAttackMode)
		{
			if (SpawnGhost(GameInstance->GhostDifficulty))
			{
				UE_LOG(LogTemp, Warning, TEXT("Ghost spawned"))
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Ghost could not spawn"))
			}
		}
	}
}


void ARacingGameGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ARacingGameGameModeBase::SaveGame(ESaveType SaveType)
{
	UE_LOG(LogTemp, Warning, TEXT("SaveGame()"))
	switch (SaveType)
	{
	case GameState:
		{
			// Save the entire game
			if (!GameInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Could not get UGlobalVariables class object. Returning."))
				return false;
			}

			// Using this to make typing faster...
			UGlobal_Variables* GV = GameInstance;
		
			UGameInstanceSaveGame* SGI = Cast<UGameInstanceSaveGame>(UGameplayStatics::CreateSaveGameObject(UGameInstanceSaveGame::StaticClass()));

			if (!SGI) { return false; }
		
			// Loading information into save object
			SGI->CurrentHealth = GV->CurrentHealth;
			SGI->MaxHealth = GV->MaxHealth;
			SGI->MaxAmmo = GV->MaxAmmo;
			SGI->CurrentAmmo = GV->CurrentAmmo;
			SGI->AmmoPercentage = GV->AmmoPercentage;
			SGI->Currency1 = GV->Currency1;
			SGI->Currency2 = GV->Currency2;
			SGI->BoostPickup = GV->BoostPickup;
			SGI->MaxBoost = GV->MaxBoost;
			SGI->TimeCount = GV->TimeCount;
			SGI->TimeAdded = GV->TimeAdded;
			SGI->DeltaTimeCount = GV->DeltaTimeCount;
			SGI->HealthActivate = GV->HealthActivate;
			SGI->HealthUpgradeTier = GV->HealthUpgradeTier;
			SGI->HealthUpgradeCost = GV->HealthUpgradeCost;
			SGI->AmmoActivate = GV->AmmoActivate;
			SGI->AmmoUpgradeTier = GV->AmmoUpgradeTier;
			SGI->AmmoUpgradeCost = GV->AmmoUpgradeCost;
			SGI->BoostActivate = GV->BoostActivate;
			SGI->BoostUpgradeTier = GV->BoostUpgradeTier;
			SGI->BoostUpgradeCost = GV->BoostUpgradeCost;
			SGI->TimeActivate = GV->TimeActivate;
			SGI->TimeUpgradeTier = GV->TimeUpgradeTier;
			SGI->TimeUpgradeCost = GV->TimeUpgradeCost;
			SGI->Health_Display = GV->Health_Display;
			SGI->Boost_Display = GV->Boost_Display;
			SGI->Ammo_Display = GV->Ammo_Display;
			SGI->Time_Display = GV->Time_Display;
			SGI->Currency1_Display = GV->Currency1_Display;
			SGI->Currency2_Display = GV->Currency2_Display;
			SGI->Speed_Display = GV->Speed_Display;
			SGI->RealTimer_Display = GV->RealTimer_Display;
			SGI->Laps_Display = GV->Laps_Display;
			SGI->Difficulty_Easy = GV->Difficulty_Easy;
			SGI->Difficulty_Normal = GV->Difficulty_Normal;
			SGI->Difficulty_Hard = GV->Difficulty_Hard;
			SGI->GlobalVolumeMultiplier = GV->GlobalVolumeMultiplier;

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
			UE_LOG(LogTemp, Warning, TEXT("Saving game... Location Arr size: %d"), LocationArr.Num())
			SaveGameInstance->LocationArr = LocationArr;
			SaveGameInstance->RotationArr = RotationArr;
			UE_LOG(LogTemp, Warning, TEXT("Saved game... Location Arr size in saveinstance: %d"), SaveGameInstance->LocationArr.Num())

			if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("GhostSave"), 0))
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully saved the ghost image. Recording size: %d"), LocationArr.Num())
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

bool ARacingGameGameModeBase::LoadGameState()
{
	// Load the entire game
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get UGlobalVariables class object. Returning."))
		return false;
	}

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("GameState"), 0)) { return false; }

	UGlobal_Variables* GV = GameInstance;
	UGameInstanceSaveGame* SGI = Cast<UGameInstanceSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GameState"), 0));

	if (!SGI) { return false; }

	GV->CurrentHealth=SGI->CurrentHealth;
	GV->MaxHealth= SGI->MaxHealth;
	GV->MaxAmmo= SGI->MaxAmmo;
	GV->CurrentAmmo= SGI->CurrentAmmo;
	GV->AmmoPercentage= SGI->AmmoPercentage;
	GV->Currency1= SGI->Currency1;
	GV->Currency2= SGI->Currency2;
	GV->BoostPickup= SGI->BoostPickup;
	GV->MaxBoost= SGI->MaxBoost;
	GV->TimeCount= SGI->TimeCount;
	GV->TimeAdded= SGI->TimeAdded;
	GV->DeltaTimeCount= SGI->DeltaTimeCount;
	GV->HealthActivate= SGI->HealthActivate;
	GV->HealthUpgradeTier= SGI->HealthUpgradeTier;
	GV->HealthUpgradeCost= SGI->HealthUpgradeCost;
	GV->AmmoActivate= SGI->AmmoActivate;
	GV->AmmoUpgradeTier= SGI->AmmoUpgradeTier;
	GV->AmmoUpgradeCost= SGI->AmmoUpgradeCost;
	GV->BoostActivate= SGI->BoostActivate;
	GV->BoostUpgradeTier= SGI->BoostUpgradeTier;
	GV->BoostUpgradeCost= SGI->BoostUpgradeCost;
	GV->TimeActivate= SGI->TimeActivate;
	GV->TimeUpgradeTier= SGI->TimeUpgradeTier;
	GV->TimeUpgradeCost= SGI->TimeUpgradeCost;
	GV->Health_Display= SGI->Health_Display;
	GV->Boost_Display= SGI->Boost_Display;
	GV->Ammo_Display= SGI->Ammo_Display;
	GV->Time_Display= SGI->Time_Display;
	GV->Currency1_Display= SGI->Currency1_Display;
	GV->Currency2_Display= SGI->Currency2_Display;
	GV->Speed_Display= SGI->Speed_Display;
	GV->RealTimer_Display= SGI->RealTimer_Display;
	GV->Laps_Display= SGI->Laps_Display;
	GV->Difficulty_Easy= SGI->Difficulty_Easy;
	GV->Difficulty_Normal= SGI->Difficulty_Normal;
	GV->Difficulty_Hard= SGI->Difficulty_Hard;
	GV->GlobalVolumeMultiplier= SGI->GlobalVolumeMultiplier;
	return true;
}

bool ARacingGameGameModeBase::LoadGhost(int32 Difficulty)
{
	//Create an instance of the save-game class

	if (!UGameplayStatics::DoesSaveGameExist(TEXT("GhostSave"), Difficulty)) { return false; }
		
	UGhostImageSaveGame* SaveGameInstance;
	SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GhostSave"), Difficulty));
	if (SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameInstance in loadgame is valid. LocationArr size: %d  %s"), SaveGameInstance->LocationArr.Num(), *SaveGameInstance->SaveName)
		LocationArr = SaveGameInstance->LocationArr;
		RotationArr = SaveGameInstance->RotationArr;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadGame failed."))
		return false;
	}
}

void ARacingGameGameModeBase::StartRace()
{
	// Start race
	if (GameInstance)
	{
		GameInstance->bRaceNotStarted = false;
	}
	
	if (GhostShipRef)
	{
		GhostShipRef->StartPlayback();
	}
	UE_LOG(LogTemp, Warning, TEXT("Starting race"))
}

void ARacingGameGameModeBase::StartRecording()
{
	bIsRecording = true;

	LocationArr.Empty();
	RotationArr.Empty();

	// The function "RecordTick" is now running in a loop via the "RecordingTimerHandle".
	GetWorld()->GetTimerManager().SetTimer(RecordingTimerHandle, this, &ARacingGameGameModeBase::RecordTick, 0.0166666667f, true);
}

void ARacingGameGameModeBase::StopRecording()
{
	bIsRecording = false;

	GetWorld()->GetTimerManager().ClearTimer(RecordingTimerHandle);

	SaveGame(GhostImage);

	LocationArr.Empty();
	RotationArr.Empty();
}

void ARacingGameGameModeBase::RecordTick()
{
	LocationArr.Add(PlayerShipRef->GetActorLocation());
	RotationArr.Add(PlayerShipRef->GetActorRotation());
}

bool ARacingGameGameModeBase::SpawnGhost(int32 Difficulty)
{
	// Load ghost
	LocationArr.Empty();
	RotationArr.Empty();

	if (!LoadGhost(Difficulty))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not spawn as loadgame returned false."))
		return false;
	}

	if (LocationArr.Num() == 0 || RotationArr.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Arr is 0! Returning."))
		return false;
	}
	
	// Spawn the actor
	if (GhostShipClass)
	{
		GhostShipRef = GetWorld()->SpawnActor<AGhostImageShip>(GhostShipClass, LocationArr[0], RotationArr[0]);
	}
	
	// Check if the actor was successfully spawned
	if (GhostShipRef)
	{
		// Here we have the reference/pointer to the class object
		GhostShipRef = Cast<AGhostImageShip>(GhostShipRef);

		if (GhostShipRef)
		{
			GhostShipRef->LocationArr = LocationArr;
			GhostShipRef->RotationArr = RotationArr;
			return true;
		} 
	}

	return false;
}

void ARacingGameGameModeBase::ChangeLevel()
{
	//Change Level
	FName NewLevel = FName("Master_Map");
	UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
}

