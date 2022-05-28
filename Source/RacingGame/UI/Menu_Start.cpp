// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Start.h"
#include "../Global_Variables.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../RacingGameGameModeBase.h"
#include "Components/CanvasPanel.h"

void UMenu_Start::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Start::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->Racing = false;
		GameInstance->bRaceNotStarted = false;
		if (GameInstance->bIsGameLoaded)
		{
			bool saved = GameInstance->SaveGame(UGlobal_Variables::GameState);
			UE_LOG(LogTemp, Warning, TEXT("Ran savegame function, hilsen Menu_Start"))
			if (saved)
			{
				UE_LOG(LogTemp, Warning, TEXT("SUCESSFULLY SAVED!"))
			}
		}

		if (!GameInstance->bIsGameLoaded)
		{
			UE_LOG(LogTemp, Warning, TEXT("Started loading game"))
			GameInstance->LoadGameState();
			GameInstance->bIsGameLoaded = true;
		}
		else
		{
			EntryPanel->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	

	//Buttons
	Start_Button->OnClicked.AddDynamic(this, &UMenu_Start::Start_Button_Clickd);
	Shop_Button->OnClicked.AddDynamic(this, &UMenu_Start::Shop_Button_Clickd);
	Settings_Button->OnClicked.AddDynamic(this, &UMenu_Start::Settings_Button_Clickd);
	Quit_Button->OnClicked.AddDynamic(this, &UMenu_Start::Quit_Button_Clickd);
	EntryButton->OnClicked.AddDynamic(this, &UMenu_Start::RemoveEntryScreen);
	
	GameInstance->PlayerCheckpointTime_Array.Empty();
	GameInstance->PlayerCheckpointTime_Array.Init(0, 100);

	GameInstance->GhostCheckpointTime.Empty();
	GameInstance->GhostCheckpointTime.Init(0, 100);
}
void UMenu_Start::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{	
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Start::Start_Button_Clickd()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectSound, 1.f * GameInstance->GlobalVolumeMultiplier);
		/*Menu_Music_AudioComponent->SetActive(false);*/
	}
}
void UMenu_Start::Shop_Button_Clickd()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectSound, 1.f * GameInstance->GlobalVolumeMultiplier);
	}
}
void UMenu_Start::Settings_Button_Clickd()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectSound, 1.f * GameInstance->GlobalVolumeMultiplier);
	}
}
void UMenu_Start::Quit_Button_Clickd()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SelectSound, 1.f * GameInstance->GlobalVolumeMultiplier);

		TEnumAsByte<EQuitPreference::Type> Quit;
		UKismetSystemLibrary::QuitGame(GetWorld(), 0, Quit, false);
	}
}

void UMenu_Start::RemoveEntryScreen()
{
	EntryPanel->SetVisibility(ESlateVisibility::Hidden);
}

