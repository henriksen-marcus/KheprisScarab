// Fill out your copyright notice in the Description page of Project Settings.


#include "LoseScreen.h"
#include "Global_Variables.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"


void ULoseScreen::NativeConstruct()
{
	Super::NativeConstruct();
}
void ULoseScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons
	Back_Button->OnClicked.AddDynamic(this, &ULoseScreen::Back_Button_Pressed);

}
void ULoseScreen::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//



void ULoseScreen::Back_Button_Pressed()
{
	UE_LOG(LogTemp, Warning, TEXT("LoseScreen - Back Button pressed"));

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	GameInstance->Pause = false;

	//Return to Start Screen
	GameInstance->CurrentLap_Counter = 0;
	GameInstance->Racing = false;
	GameInstance->bRaceNotStarted = true;

	//Change Level
	FName NewLevel = FName("Start_Screen");
	UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
}