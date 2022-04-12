// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Settings.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UMenu_Settings::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Settings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons
	Back_Button->OnClicked.AddDynamic(this, &UMenu_Settings::Back_Button_Clicked);

	//Text on Buttons
	Back_Button_Text->SetText(FText::FromString("Back"));

}
void UMenu_Settings::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
}


//------------------------ OWN FUNCTIONS ------------------------//

void UMenu_Settings::Back_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BackSound);
}

