// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Start.h"
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

	//Buttons
	Start_Button->OnClicked.AddDynamic(this, &UMenu_Start::Start_Button_Clickd);
	Shop_Button->OnClicked.AddDynamic(this, &UMenu_Start::Shop_Button_Clickd);
	Settings_Button->OnClicked.AddDynamic(this, &UMenu_Start::Settings_Button_Clickd);
	Quit_Button->OnClicked.AddDynamic(this, &UMenu_Start::Quit_Button_Clickd);

	//Text on Buttons
	Start_Button_Text->SetText(FText::FromString("Start"));
	Shop_Button_Text->SetText(FText::FromString("Shop"));
	Settings_Button_Text->SetText(FText::FromString("Settings"));
	Quit_Button_Text->SetText(FText::FromString("Quit"));
}
void UMenu_Start::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Start::Start_Button_Clickd()
{
	UGameplayStatics::PlaySound2D(GetWorld(), SelectSound);
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

