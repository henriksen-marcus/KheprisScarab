// Copyright Epic Games, Inc. All Rights Reserved.


#include "RacingGameGameModeBase.h"

#include <string>

#include "Global_Variables.h"
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

	GlobalVariables = Cast<UGlobal_Variables>(GetGameInstance());
	
	/**
	 * The point of latent actions are that they can take more than one frame to execute and then will trigger a function when finished.
	 * Fill out the struct
	 */
	/*TimeStart = GetWorld()->TimeSeconds;
	FLatentActionInfo Info;
	Info.CallbackTarget = this;
	Info.ExecutionFunction = FName("UnloadCompleted");
	Info.Linkage = 0;
	Info.UUID = 0;
	UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("Main"), Info, false);*/
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
		// Save the entire game
		if (!GlobalVariables)
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not get UGlobalVariables class object. Returning."))
			return false;
		}
		
		
		break;
	case GhostImage:
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
		SaveGameInstance1 = Cast<UGhostImageSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("GhostSave"), 0));
		if (SaveGameInstance1)
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveGameInstance in loadgame is valid. LocationArr size: %d  %s"), SaveGameInstance1->LocationArr.Num(), *SaveGameInstance1->SaveName)
			LocationArr = SaveGameInstance1->LocationArr;
			RotationArr = SaveGameInstance1->RotationArr;
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
	UE_LOG(LogTemp, Warning, TEXT("Recording!"))
}


bool ARacingGameGameModeBase::SpawnGhost()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnGhost()"))

	// Load ghost
	LocationArr.Empty();
	RotationArr.Empty();

	if (!LoadGame(GhostImage))
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
			GhostShipRef->StartPlayback();
			return true;
		} 
	}

	return false;
}

void ARacingGameGameModeBase::UnloadCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Unload Completed, Time taken: %f"), GetWorld()->TimeSeconds - TimeStart);
}

