// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Settings.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Engine/Texture2D.h"
#include "RacingGame/Global_Variables.h"

void UMenu_Settings::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Settings::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Buttons
	Back_Button->OnClicked.AddDynamic(this, &UMenu_Settings::Back_Button_Clicked);
	HealthDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::HealthDisplay_Button_Clicked);
	AmmoDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::AmmoDisplay_Button_Clicked);
	BoostDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::BoostDisplay_Button_Clicked);
	TimeDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::TimeDisplay_Button_Clicked);
	Currency1Display_Button->OnClicked.AddDynamic(this, &UMenu_Settings::Currency1Display_Button_Clicked);
	Currency2Display_Button->OnClicked.AddDynamic(this, &UMenu_Settings::Currency2Display_Button_Clicked);
	SpeedDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::SpeedDisplay_Button_Clicked);
	RealTimeDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::RealTimeDisplay_Button_Clicked);
	LapsDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::LapsDisplay_Button_Clicked);

	EasyDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::EasyDisplay_Button_Clicked);
	NormalDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::NormalDisplay_Button_Clicked);
	HardDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::HardDisplay_Button_Clicked);
	
	GlobalVolumeSlider->OnValueChanged.AddDynamic(this, &UMenu_Settings::UpdateGlobalVolume);


	GlobalVars = Cast<UGlobal_Variables>(GetGameInstance());
	
	if (GlobalVars)
	{
		GlobalVolumeSlider->SetValue(GlobalVars->GlobalVolumeMultiplier);
		const FString Str = FString::FormatAsNumber(GlobalVars->GlobalVolumeMultiplier * 100) + "%";
		GlobalVolumeNumber->SetText(FText::FromString(Str));
	}
	

}
void UMenu_Settings::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	SetBoxesSelected();
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Settings::SetBoxesSelected()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		#pragma region HUD Display
		if (GameInstance->Health_Display == true)
			HealthDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			HealthDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Ammo_Display == true)
			AmmoDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			AmmoDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Boost_Display == true)
			BoostDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			BoostDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Time_Display == true)
			TimeDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			TimeDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Currency1_Display == true)
			Currency1Display_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			Currency1Display_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Currency2_Display == true)
			Currency2Display_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			Currency2Display_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Speed_Display == true)
			SpeedDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			SpeedDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->RealTimer_Display == true)
			RealTimeDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			RealTimeDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (GameInstance->Laps_Display == true)
			LapsDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			LapsDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
		#pragma endregion

		#pragma region Difficulty
		if (GameInstance->Difficulty_Easy == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
		}
		else if (GameInstance->Difficulty_Normal == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
		}
		else if (GameInstance->Difficulty_Hard == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		}
		#pragma endregion
	}
}

void UMenu_Settings::Back_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BackSound, 1.f * GameInstance->GlobalVolumeMultiplier);
	}
}

//Edited to global variable
void UMenu_Settings::HealthDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Health_Display == true)
			GameInstance->Health_Display = false;
		else
			GameInstance->Health_Display = true;
	}
}
void UMenu_Settings::AmmoDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Ammo_Display == true)
			GameInstance->Ammo_Display = false;
		else
			GameInstance->Ammo_Display = true;
	}
}
void UMenu_Settings::BoostDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Boost_Display == true)
			GameInstance->Boost_Display = false;
		else
			GameInstance->Boost_Display = true;
	}
}
void UMenu_Settings::TimeDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Time_Display == true)
			GameInstance->Time_Display = false;
		else
			GameInstance->Time_Display = true;
	}
}
void UMenu_Settings::Currency1Display_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Currency1_Display == true)
			GameInstance->Currency1_Display = false;
		else
			GameInstance->Currency1_Display = true;
	}
}
void UMenu_Settings::Currency2Display_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Currency2_Display == true)
			GameInstance->Currency2_Display = false;
		else
			GameInstance->Currency2_Display = true;
	}
}
void UMenu_Settings::SpeedDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Speed_Display == true)
			GameInstance->Speed_Display = false;
		else
			GameInstance->Speed_Display = true;
	}
}
void UMenu_Settings::RealTimeDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->RealTimer_Display == true)
			GameInstance->RealTimer_Display = false;
		else
			GameInstance->RealTimer_Display = true;
	}
}
void UMenu_Settings::LapsDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->Laps_Display == true)
			GameInstance->Laps_Display = false;
		else
			GameInstance->Laps_Display = true;
	}
}

void UMenu_Settings::EasyDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		GameInstance->Difficulty_Easy = true;
		GameInstance->Difficulty_Normal = false;
		GameInstance->Difficulty_Hard = false;

		GameInstance->TimeStartCount = 75;
	}
}
void UMenu_Settings::NormalDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		GameInstance->Difficulty_Easy = false;
		GameInstance->Difficulty_Normal = true;
		GameInstance->Difficulty_Hard = false;

		GameInstance->TimeStartCount = 60;
	}
}
void UMenu_Settings::HardDisplay_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);

		GameInstance->Difficulty_Easy = false;
		GameInstance->Difficulty_Normal = false;
		GameInstance->Difficulty_Hard = true;

		GameInstance->TimeStartCount = 45;
	}
}

void UMenu_Settings::UpdateGlobalVolume(const float NewVolume)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	
	if (GameInstance)
	{
		FString Str = FString::FormatAsNumber(NewVolume * 100) + "%";
		GlobalVolumeNumber->SetText(FText::FromString(Str));
		GameInstance->GlobalVolumeMultiplier = NewVolume;
	}
}
