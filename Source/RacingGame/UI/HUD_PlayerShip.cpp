// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_PlayerShip.h"
#include "../Vehicles/PlayerShip.h"
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
	SetTimer_Display();
}


//-------------- OWN FUNCTIONS --------------//


void UHUD_PlayerShip::SetHUDVisibility()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Health_Display == true)
			PlayerHealth_Bar->SetVisibility(ESlateVisibility::Visible);
		else
			PlayerHealth_Bar->SetVisibility(ESlateVisibility::Hidden);

		if (PlayerShip->Ammo_Display == true)
			Ammo_Bar->SetVisibility(ESlateVisibility::Visible);
		else
			Ammo_Bar->SetVisibility(ESlateVisibility::Hidden);

		if (PlayerShip->Boost_Display == true)
			Boost_Image->SetVisibility(ESlateVisibility::Visible);
		else
			Boost_Image->SetVisibility(ESlateVisibility::Hidden);

		if (PlayerShip->Time_Display == true)
			Time_Text->SetVisibility(ESlateVisibility::Visible);
		else
			Time_Text->SetVisibility(ESlateVisibility::Hidden);

		if (PlayerShip->Currency1_Display == true)
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Visible);
			Currency1_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Hidden);
			Currency1_Text->SetVisibility(ESlateVisibility::Hidden);
		}

		if (PlayerShip->Currency2_Display == true)
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
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		Player_Precentage = PlayerShip->CurrentHealth / PlayerShip->MaxHealth;
		PlayerHealth_Bar->SetPercent(Player_Precentage);
	}
}

void UHUD_PlayerShip::SetCurrency1_Display()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		FString PlayerCurrency1 = UKismetStringLibrary::Conv_IntToString(PlayerShip->Currency1);
		Currency1_Text->SetText(FText::FromString(PlayerCurrency1));
	}
}
void UHUD_PlayerShip::SetCurrency2_Display()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		FString PlayerCurrency2 = UKismetStringLibrary::Conv_IntToString(PlayerShip->Currency2);
		Currency2_Text->SetText(FText::FromString(PlayerCurrency2));
	}
}

void UHUD_PlayerShip::Boost_Display()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->BoostPickup == true)
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
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		PlayerShip->AmmoPrecentage = PlayerShip->CurrentAmmo / PlayerShip->MaxAmmo;
		Ammo_Bar->SetPercent(PlayerShip->AmmoPrecentage);
	}
}

void UHUD_PlayerShip::SetTimer_Display()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		FString Timer = UKismetStringLibrary::Conv_IntToString(PlayerShip->TimeCount);
		Time_Text->SetText(FText::FromString(Timer));
	}
}
