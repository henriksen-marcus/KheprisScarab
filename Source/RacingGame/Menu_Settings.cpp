// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Settings.h"
#include "Components/TextBlock.h"

void UMenu_Settings::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Settings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons

	//Text on Buttons
	Back_Button_Text->SetText(FText::FromString("Back"));

}
void UMenu_Settings::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Settings::Test_Button_Clickd()
{
}