// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_ModeSelect.h"
#include "../Global_Variables.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"


void UMenu_ModeSelect::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_ModeSelect::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons
	TimeAttack_Button->OnClicked.AddDynamic(this, &UMenu_ModeSelect::TimeAttack_Button_Clicked);
	Adventure_Button->OnClicked.AddDynamic(this, &UMenu_ModeSelect::Adventure_Button_Clicked);
	Back_Button->OnClicked.AddDynamic(this, &UMenu_ModeSelect::Back_Button_Clicked);


}
void UMenu_ModeSelect::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{

}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_ModeSelect::TimeAttack_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
		GameInstance->TimeAttackMode = true;
	}
}
void UMenu_ModeSelect::Adventure_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
		GameInstance->TimeAttackMode = false;
	}
}

void UMenu_ModeSelect::Back_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BackSound, 1.f * GameInstance->GlobalVolumeMultiplier);
	}
}
