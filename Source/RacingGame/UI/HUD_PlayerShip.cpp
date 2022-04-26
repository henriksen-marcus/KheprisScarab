// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_PlayerShip.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UHUD_PlayerShip::NativeConstruct()
{
	Super::NativeConstruct();
}
void UHUD_PlayerShip::NativeOnInitialized()
{
}
void UHUD_PlayerShip::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	SetHUDVisibility();

	SetBarPercentage();
	SetCurrency1_Display();
	SetCurrency2_Display();
	Boost_Display();
	SetAmmoPrecentage();

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->TimeAttackMode == true)
		{
			SetTimer_Display();

			SetTimer(DeltaTime);
		}
	}
}


//-------------- OWN FUNCTIONS --------------//


void UHUD_PlayerShip::SetHUDVisibility()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->Health_Display == true)
			PlayerHealth_Bar->SetVisibility(ESlateVisibility::Visible);
		else
			PlayerHealth_Bar->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->Ammo_Display == true)
			Ammo_Bar->SetVisibility(ESlateVisibility::Visible);
		else
			Ammo_Bar->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->Boost_Display == true)
			Boost_Image->SetVisibility(ESlateVisibility::Visible);
		else
			Boost_Image->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->Time_Display == true)
			Time_Text->SetVisibility(ESlateVisibility::Visible);
		else
			Time_Text->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->Currency1_Display == true)
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Visible);
			Currency1_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Hidden);
			Currency1_Text->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GameInstance->Currency2_Display == true)
		{
			Currency2_Image->SetVisibility(ESlateVisibility::Visible);
			Currency2_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Currency2_Image->SetVisibility(ESlateVisibility::Hidden);
			Currency2_Text->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UHUD_PlayerShip::SetBarPercentage()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		Player_Precentage = GameInstance->CurrentHealth / GameInstance->MaxHealth;
		PlayerHealth_Bar->SetPercent(Player_Precentage);
	}
}

void UHUD_PlayerShip::SetCurrency1_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		FString PlayerCurrency1 = UKismetStringLibrary::Conv_IntToString(GameInstance->Currency1);
		Currency1_Text->SetText(FText::FromString(PlayerCurrency1));
	}
}
void UHUD_PlayerShip::SetCurrency2_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		FString PlayerCurrency2 = UKismetStringLibrary::Conv_IntToString(GameInstance->Currency2);
		Currency2_Text->SetText(FText::FromString(PlayerCurrency2));
	}
}

void UHUD_PlayerShip::Boost_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->BoostPickup == true)
		{
			Boost_Image->SetBrushFromTexture(Boost_Active);
		}
		else
		{
			Boost_Image->SetBrushFromTexture(Boost_Unactive);
		}
	}
}

void UHUD_PlayerShip::SetAmmoPrecentage()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AmmoPrecentage = GameInstance->CurrentAmmo / GameInstance->MaxAmmo;
		Ammo_Bar->SetPercent(GameInstance->AmmoPrecentage);
	}
}

void UHUD_PlayerShip::SetTimer_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		FString Timer = UKismetStringLibrary::Conv_IntToString(GameInstance->TimeCount);
		Time_Text->SetText(FText::FromString(Timer));
	}
}

void UHUD_PlayerShip::SetTimer(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());

	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			GameInstance->DeltaTimeCount += DeltaTime;

			if (GameInstance->DeltaTimeCount >= 1)
			{
				GameInstance->DeltaTimeCount = 0;

				GameInstance->TimeCount -= 1;
			}
		}
	}
}
