// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Start.h"
#include "../Global_Variables.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"

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
	}

	//Buttons
	Start_Button->OnClicked.AddDynamic(this, &UMenu_Start::Start_Button_Clickd);
	Shop_Button->OnClicked.AddDynamic(this, &UMenu_Start::Shop_Button_Clickd);
	Settings_Button->OnClicked.AddDynamic(this, &UMenu_Start::Settings_Button_Clickd);
	Quit_Button->OnClicked.AddDynamic(this, &UMenu_Start::Quit_Button_Clickd);

	//Music

}
void UMenu_Start::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{	
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Start::Start_Button_Clickd()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SelectSound);
	/*Menu_Music_AudioComponent->SetActive(false);*/
}
void UMenu_Start::Shop_Button_Clickd()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SelectSound);
}
void UMenu_Start::Settings_Button_Clickd()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SelectSound);
}
void UMenu_Start::Quit_Button_Clickd()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SelectSound);

	TEnumAsByte<EQuitPreference::Type> Quit;
	UKismetSystemLibrary::QuitGame(GetWorld(), 0, Quit, false);
}

