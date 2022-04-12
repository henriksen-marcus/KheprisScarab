// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Shop.h"
#include "PlayerShip.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"

void UMenu_Shop::NativeConstruct()
{
	Super::NativeConstruct();
}
void UMenu_Shop::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//Variables
	HealthUpgradeCost = 2;
	AmmoUpgradeCost = 2;
	BoostUpgradeCost = 2;
	TimeUpgradeCost = 2;

	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		PlayerShip->Currency1 = 100;
		PlayerShip->Currency2 = 3;
	}


	//------------------------------


	//Text on Buttons
	Back_Button_Text->SetText(FText::FromString("Back"));

	//Buttons Clicked
	HealthShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Clicked);
	AmmoShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Clicked);
	BoostShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Clicked);
	TimeShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Clicked);
	Back_Button->OnClicked.AddDynamic(this, &UMenu_Shop::Back_Button_Clicked);
}
void UMenu_Shop::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Currency_Dispaly();
	SetHealthUpgrade_Display();
	SetAmmoUpgrade_Display();
	SetBoostUpgrade_Display();
	SetTimeUpgrade_Display();
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Shop::Currency_Dispaly()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		Currency1_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(PlayerShip->Currency1)));
		Currency2_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(PlayerShip->Currency2)));
	}
}

void UMenu_Shop::Back_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BackSound);
}

void UMenu_Shop::SetHealthUpgrade_Display()
{
	#pragma region Image
	if (HealthUpgradeTier <= 0)
		HealthUpgrade_Image->SetBrushFromTexture(Base);
	else if (HealthUpgradeTier == 1)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade1);
	else if (HealthUpgradeTier == 2)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade2);
	else if (HealthUpgradeTier == 3)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade3);
	else if (HealthUpgradeTier == 4)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade4);
	else if (HealthUpgradeTier == 5)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade5);
	else if (HealthUpgradeTier == 6)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade6);
	else if (HealthUpgradeTier == 7)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade7);
	else if (HealthUpgradeTier == 8)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade8);
	else if (HealthUpgradeTier == 9)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade9);
	else if (HealthUpgradeTier >= 10)
		HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade10);
	#pragma endregion
	
	if (HealthUpgradeTier >= 10)
	{
		HealthShop_Button->SetVisibility(ESlateVisibility::Hidden);
		Health_Info_Text->SetText(FText::FromString("Your Health has reached its max"));
	}

	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (HealthActivate == true)
			HealthShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(HealthUpgradeCost)));
		else
			HealthShop_Button_Text->SetText(FText::FromString("Locked"));
		
		MaxHealth_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(PlayerShip->MaxHealth)));
	}
}
void UMenu_Shop::HealthShop_Button_Clicked()
{
	if (HealthActivate == true)
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (HealthUpgradeTier < 10)
			{
				if (PlayerShip->Currency1 >= HealthUpgradeCost)
				{
					if (HealthUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound);

					PlayerShip->Currency1 -= HealthUpgradeCost;
					HealthUpgradeTier += 1;
					HealthUpgradeCost += 1;
					PlayerShip->MaxHealth += 1;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound);
			}
		}
	}
	else
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (PlayerShip->Currency2 >= 1)
			{
				HealthActivate = true;
				PlayerShip->Currency2 -= 1;

				UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound);
			}
			else
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		}
	}
}

void UMenu_Shop::SetAmmoUpgrade_Display()
{
	#pragma region Image
	if (AmmoUpgradeTier <= 0)
		AmmoUpgrade_Image->SetBrushFromTexture(Base);
	else if (AmmoUpgradeTier == 1)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade1);
	else if (AmmoUpgradeTier == 2)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade2);
	else if (AmmoUpgradeTier == 3)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade3);
	else if (AmmoUpgradeTier == 4)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade4);
	else if (AmmoUpgradeTier == 5)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade5);
	else if (AmmoUpgradeTier == 6)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade6);
	else if (AmmoUpgradeTier == 7)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade7);
	else if (AmmoUpgradeTier == 8)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade8);
	else if (AmmoUpgradeTier == 9)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade9);
	else if (AmmoUpgradeTier >= 10)
		AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade10);
	#pragma endregion

	if (AmmoUpgradeTier >= 10)
	{
		AmmoShop_Button->SetVisibility(ESlateVisibility::Hidden);
		Ammo_Info_Text->SetText(FText::FromString("Your Ammo has reached its max"));
	}

	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (AmmoActivate == true)
			AmmoShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(AmmoUpgradeCost)));
		else
			AmmoShop_Button_Text->SetText(FText::FromString("Locked"));

		MaxAmmo_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(PlayerShip->MaxAmmo)));
	}
}
void UMenu_Shop::AmmoShop_Button_Clicked()
{
	if (AmmoActivate == true)
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (AmmoUpgradeTier < 10)
			{
				if (PlayerShip->Currency1 >= AmmoUpgradeCost)
				{
					if (AmmoUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound);

					PlayerShip->Currency1 -= AmmoUpgradeCost;
					AmmoUpgradeTier += 1;
					AmmoUpgradeCost += 1;
					PlayerShip->MaxAmmo += 5;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound);
			}
		}
	}
	else
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (PlayerShip->Currency2 >= 1)
			{
				AmmoActivate = true;
				PlayerShip->Currency2 -= 1;

				UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound);
			}
			else
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		}
	}
}

void UMenu_Shop::SetBoostUpgrade_Display()
{
	#pragma region Image
	if (BoostUpgradeTier <= 0)
		BoostUpgrade_Image->SetBrushFromTexture(Base);
	else if (BoostUpgradeTier == 1)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade1);
	else if (BoostUpgradeTier == 2)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade2);
	else if (BoostUpgradeTier == 3)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade3);
	else if (BoostUpgradeTier == 4)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade4);
	else if (BoostUpgradeTier == 5)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade5);
	else if (BoostUpgradeTier == 6)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade6);
	else if (BoostUpgradeTier == 7)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade7);
	else if (BoostUpgradeTier == 8)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade8);
	else if (BoostUpgradeTier == 9)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade9);
	else if (BoostUpgradeTier >= 10)
		BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade10);
	#pragma endregion

	if (BoostUpgradeTier >= 10)
	{
		BoostShop_Button->SetVisibility(ESlateVisibility::Hidden);
		Boost_Info_Text->SetText(FText::FromString("Your Boost has reached its max"));
	}

	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (BoostActivate == true)
			BoostShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(BoostUpgradeCost)));
		else
			BoostShop_Button_Text->SetText(FText::FromString("Locked"));

		MaxBoost_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_FloatToString(PlayerShip->DashTimer)));
	}
}
void UMenu_Shop::BoostShop_Button_Clicked()
{
	if (BoostActivate == true)
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (BoostUpgradeTier < 10)
			{
				if (PlayerShip->Currency1 >= BoostUpgradeCost)
				{
					if (BoostUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound);

					PlayerShip->Currency1 -= BoostUpgradeCost;
					BoostUpgradeTier += 1;
					BoostUpgradeCost += 1;
					PlayerShip->DashTimer += 0.5f;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound);
			}
		}
	}
	else
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (PlayerShip->Currency2 >= 1)
			{
				BoostActivate = true;
				PlayerShip->Currency2 -= 1;

				UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound);
			}
			else
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		}
	}
}

void UMenu_Shop::SetTimeUpgrade_Display()
{
	#pragma region Image
	if (TimeUpgradeTier <= 0)
		TimeUpgrade_Image->SetBrushFromTexture(Base);
	else if (TimeUpgradeTier == 1)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade1);
	else if (TimeUpgradeTier == 2)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade2);
	else if (TimeUpgradeTier == 3)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade3);
	else if (TimeUpgradeTier == 4)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade4);
	else if (TimeUpgradeTier == 5)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade5);
	else if (TimeUpgradeTier == 6)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade6);
	else if (TimeUpgradeTier == 7)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade7);
	else if (TimeUpgradeTier == 8)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade8);
	else if (TimeUpgradeTier == 9)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade9);
	else if (TimeUpgradeTier >= 10)
		TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade10);
	#pragma endregion

	if (TimeUpgradeTier >= 10)
	{
		TimeShop_Button->SetVisibility(ESlateVisibility::Hidden);
		Time_Info_Text->SetText(FText::FromString("Your Extra Time has reached its max"));
	}

	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (TimeActivate == true)
			TimeShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(TimeUpgradeCost)));
		else
			TimeShop_Button_Text->SetText(FText::FromString("Locked"));

		MaxTime_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(PlayerShip->TimeAdded)));
	}
}
void UMenu_Shop::TimeShop_Button_Clicked()
{
	if (TimeActivate == true)
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (TimeUpgradeTier < 10)
			{
				if (PlayerShip->Currency1 >= TimeUpgradeCost)
				{
					if (TimeUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound);

					PlayerShip->Currency1 -= TimeUpgradeCost;
					TimeUpgradeTier += 1;
					TimeUpgradeCost += 1;
					PlayerShip->TimeAdded += 2;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound);
			}
		}
	}
	else
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			if (PlayerShip->Currency2 >= 1)
			{
				TimeActivate = true;
				PlayerShip->Currency2 -= 1;

				UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound);
			}
			else
				UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound);
		}
	}
}
