// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingGameGameModeBase.h"
#include "Other/GhostImageSaveGame.h"
#include "Vehicles/GhostImageShip.h"
#include "Vehicles/PlayerShipPhysics.h"
#include "Other/GameInstanceSaveGame.h"
#include "Other/GhostSpline.h"

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

	if (PlayerShipRef)
	{
		PlayerShipRef->SetActorLocation(CheckpointArrowLocation);
		PlayerShipRef->SetActorRotation(CheckpointArrowRotation);
	}

	LogGhostSaves();
}

void ARacingGameGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecording) { RecordingTimer += DeltaTime; }
}

bool ARacingGameGameModeBase::LoadGhost(int32 Difficulty)
{
	// Create an instance of the save-game class

	UE_LOG(LogTemp, Warning, TEXT("Starting loadghost, difficulty requested: %d"), Difficulty)
	
	bool bDoesSaveGameExist{};
	
	switch (Difficulty)
	{
	case 0:
		bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist(TEXT("PlayerGhost"), 0);
		break;
	case 1:
		bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist(TEXT("GoldGhost"), 0);
		break;
	case 2:
		bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist(TEXT("SilverGhost"), 0);
		break;
	case 3:
		bDoesSaveGameExist = UGameplayStatics::DoesSaveGameExist(TEXT("BronzeGhost"), 0);
		break;
	default:
		break;
	}

	if (!bDoesSaveGameExist)
	{
		UE_LOG(LogTemp, Warning, TEXT("bDoesSaveGameExist is false, returning."))
		return false;
	}
		
	UGhostImageSaveGame* SaveGameInstance;

	switch (Difficulty)
	{
	case 0:
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerGhost"), 0));
		break;
	case 1:
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GoldGhost"), 0));
		break;
	case 2:
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("SilverGhost"), 0));
		break;
	case 3:
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("BronzeGhost"), 0));
		break;
	default:
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerGhost"), 0));
		break;
	}
	
	if (SaveGameInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SaveGameInstance in loadgame is valid. LocationArr size: %d  %s"), SaveGameInstance->LocationArr.Num(), *SaveGameInstance->SaveName)
		LocationArr = SaveGameInstance->LocationArr;
		RotationArr = SaveGameInstance->RotationArr;
		RecordingTimer = SaveGameInstance->Time;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadGame failed, save object was not valid."))
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

	StartRecording();
	
	UE_LOG(LogTemp, Warning, TEXT("Starting race"))
}

void ARacingGameGameModeBase::StartRecording()
{
	bIsRecording = true;

	LocationArr.Empty();
	RotationArr.Empty();
	RecordingTimer = 0;

	// The function "RecordTick" is now running in a loop via the "RecordingTimerHandle".
	GetWorld()->GetTimerManager().SetTimer(RecordingTimerHandle, this, &ARacingGameGameModeBase::RecordTick, 0.0166666666666667f,true, true);
}

void ARacingGameGameModeBase::StopRecording()
{
	bIsRecording = false;

	GetWorld()->GetTimerManager().ClearTimer(RecordingTimerHandle);
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
			GhostShipRef->SpawnSpline()->GenerateSpline(LocationArr, RotationArr, RecordingTimer);
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

void ARacingGameGameModeBase::LogGhostSaves()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerGhost"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found player ghost"))
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find player ghost"))
	}
	if (UGameplayStatics::DoesSaveGameExist(TEXT("GoldGhost"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found gold ghost"))
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find gold ghost"))
	}
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SilverGhost"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found silver ghost"))
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find silver ghost"))
	}
	if (UGameplayStatics::DoesSaveGameExist(TEXT("BronzeGhost"), 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found bronze ghost"))
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find bronze ghost"))
	}
}

