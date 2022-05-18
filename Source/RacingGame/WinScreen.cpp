// Fill out your copyright notice in the Description page of Project Settings.


#include "WinScreen.h"
#include "Global_Variables.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HUD_PlayerShip.h"

void UWinScreen::NativeConstruct()
{
	Super::NativeConstruct();
}
void UWinScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons
	Back_Button->OnClicked.AddDynamic(this, &UWinScreen::Back_Button_Pressed);

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	#pragma region Setup Timer Display
	InGame_Timer_Temp = GameInstance->InGame_Timer;

	while (InGame_Timer_Temp >= 60)
	{
		UE_LOG(LogTemp, Warning, TEXT("WinScreen - Calculate Minutes"));
		InGame_Timer_Temp -= 60;
		Minutes += 1;
	}

	while (InGame_Timer_Temp >= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("WinScreen - Calculate Seconds"));
		InGame_Timer_Temp -= 1;
		Secounds += 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("WinScreen - Add Hundreds"));
	Hundrets = InGame_Timer_Temp;

	Hundrets = Hundrets * 100;
	Hundrets -= 2;

	#pragma endregion

	#pragma region Timer Text
	if (Minutes < 10)
		TimeDisplay_Minutes_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Minutes)));
	else
		TimeDisplay_Minutes_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Minutes)));

	if (Secounds < 10)
		TimeDisplay_Seconds_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Secounds)));
	else
		TimeDisplay_Seconds_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Secounds)));

	if (Hundrets < 10)
		TimeDisplay_Hundrets_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Hundrets)));
	else
		TimeDisplay_Hundrets_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Hundrets)));
	#pragma endregion

	#pragma region Timer text - Visible
	if (GameInstance->TimeAttackMode)
	{
		TimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Visible);
		TimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Visible);
		TimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Visible);
		_One->SetVisibility(ESlateVisibility::Visible);
		_Two->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		TimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Hidden);
		TimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Hidden);
		TimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Hidden);
		_One->SetVisibility(ESlateVisibility::Hidden);
		_Two->SetVisibility(ESlateVisibility::Hidden);
	}
	#pragma endregion

}
void UWinScreen::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//


void UWinScreen::Back_Button_Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("WinScreen - Back Button pressed"));

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) {return;}

	GameInstance->Pause = false;

	//Return to Start Screen
	GameInstance->CurrentLap_Counter = 0;
	GameInstance->Racing = false;
	GameInstance->bRaceNotStarted = true;

	//Change Level
	FName NewLevel = FName("Start_Screen");
	UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
}