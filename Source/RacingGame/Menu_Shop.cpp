// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Shop.h"
#include "Components/TextBlock.h"

void UMenu_Shop::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Shop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons

	//Text on Buttons
	Back_Button_Text->SetText(FText::FromString("Back"));
}
void UMenu_Shop::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Shop::Test_Button_Clickd()
{
}