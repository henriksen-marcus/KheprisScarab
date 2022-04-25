// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingGameGameModeBase.h"
#include "Other/GhostImageSaveGame.h"
#include "Vehicles/GhostImageShip.h"
#include "Vehicles/PlayerShipPhysics.h"

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
}


void ARacingGameGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecording)
	{
		LocationArr.Add(PlayerShipRef->GetActorLocation());
		RotationArr.Add(PlayerShipRef->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Added recording. Loc Num: %d, Rot Num: %d"), LocationArr.Num(), RotationArr.Num())
	}
}



bool ARacingGameGameModeBase::SaveGame(ESaveType SaveType)
{
	UE_LOG(LogTemp, Warning, TEXT("SaveGame()"))
	switch (SaveType)
	{
	case GameState:
		// Save the entire game
		break;
	case GhostImage:
		// Save the ghost image
		//Create an instance of the save-game class
		UGhostImageSaveGame* SaveGameInstance;
		SaveGameInstance = Cast<UGhostImageSaveGame>(UGameplayStatics::CreateSaveGameObject(UGhostImageSaveGame::StaticClass()));

		SaveGameInstance->EndLocation = PlayerShipRef->GetActorLocation();
		SaveGameInstance->SaveName = "SaveTestLol27";
		SaveGameInstance->LocationArr = LocationArr;
		SaveGameInstance->RotationArr = RotationArr;
		SaveGameInstance->TickNumbers = TickNumbers;

		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("GhostSave"), 0))
		{
			return true;
		}
		break;
	default:
		return false;
	}
	
	return false;
}

bool ARacingGameGameModeBase::LoadGame(ESaveType SaveType)
{
	switch (SaveType)
	{
	case GameState:
		// Load the entire game
		break;
	case GhostImage:
		//Create an instance of the save-game class
		UGhostImageSaveGame* SaveGameInstance1;
		SaveGameInstance1 = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot("GhostSave", 0));
		if (SaveGameInstance1)
		{
			UE_LOG(LogTemp, Warning, TEXT("LocationArray from LoadGame size: %d"), SaveGameInstance1->LocationArr.Num())
			UE_LOG(LogTemp, Warning, TEXT("RotationArray from LoadGame size: %d"), SaveGameInstance1->RotationArr.Num())
			UE_LOG(LogTemp, Warning, TEXT("Save name: %s"), *SaveGameInstance1->SaveName)
			UE_LOG(LogTemp, Warning, TEXT("End FVector: %s"), *SaveGameInstance1->EndLocation.ToString())
			LocationArr = SaveGameInstance1->LocationArr;
			RotationArr = SaveGameInstance1->RotationArr;
			TickNumbers = SaveGameInstance1->TickNumbers;
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LoadGame failed."))
		}
		
		break;
	default:
		return false;
	}
	
	return false;
}

void ARacingGameGameModeBase::StartRecording()
{
	UE_LOG(LogTemp, Warning, TEXT("StartRecording()"))
	bIsRecording = true;

	LocationArr.Empty();
	RotationArr.Empty();
}

void ARacingGameGameModeBase::StopRecording()
{
	UE_LOG(LogTemp, Warning, TEXT("StopRecording()"))
	bIsRecording = false;

	if (SaveGame(GhostImage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully saved the ghost image. Recording size: %d"), LocationArr.Num())
	}

	LocationArr.Empty();
	RotationArr.Empty();
	TickNumbers.Empty();
}

bool ARacingGameGameModeBase::SpawnGhost()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnGhost()"))

	// Load ghost
	LocationArr.Empty();
	RotationArr.Empty();
	TickNumbers.Empty();

	if (!LoadGame(GhostImage))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not spawn as loadgame returned false."))
		return false;
	}

	if (LocationArr.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LocationArr is 0! Returning."))
		return false;
	}
	
	// Spawn the actor
	GhostShipRef = GetWorld()->SpawnActor<AGhostImageShip>(AGhostImageShip::StaticClass(), LocationArr[0], RotationArr[0]);

	// Check if the actor was successfully spawned
	if (GhostShipRef)
	{
		// Here we have the reference/pointer to the class object
		GhostShipRef = Cast<AGhostImageShip>(GhostShipRef);

		if (GhostShipRef)
		{
			GhostShipRef->LocationArr = LocationArr;
			GhostShipRef->RotationArr = RotationArr;
			GhostShipRef->TickNumbers = TickNumbers;
			GhostShipRef->bRaceStarted = true;
			return true;
		} 
	}

	return false;
}
