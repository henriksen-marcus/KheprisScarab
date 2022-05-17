// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Shop.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
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


	//------------------------------


	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());

	#pragma region Button functionality
	//Health Button functionality
	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Idle);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Locked_Idle);
	HealthShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Clicked);
	HealthShop_Button->OnHovered.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Hovered);
	HealthShop_Button->OnUnhovered.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Unhovered);
	HealthShop_Button->OnPressed.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Pressed);
	HealthShop_Button->OnReleased.AddDynamic(this, &UMenu_Shop::HealthShop_Button_Realised);

	//Ammo Button functionality
	if (GameInstance->HealthActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Idle);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Locked_Idle);
	AmmoShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Clicked);
	AmmoShop_Button->OnHovered.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Hovered);
	AmmoShop_Button->OnUnhovered.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Unhovered);
	AmmoShop_Button->OnPressed.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Pressed);
	AmmoShop_Button->OnReleased.AddDynamic(this, &UMenu_Shop::AmmoShop_Button_Realised);

	//Boost Button functionality
	if (GameInstance->HealthActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Idle);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Locked_Idle);
	BoostShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Clicked);
	BoostShop_Button->OnHovered.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Hovered);
	BoostShop_Button->OnUnhovered.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Unhovered);
	BoostShop_Button->OnPressed.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Pressed);
	BoostShop_Button->OnReleased.AddDynamic(this, &UMenu_Shop::BoostShop_Button_Realised);

	//Time Button functionality
	if (GameInstance->HealthActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Idle);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Locked_Idle);
	TimeShop_Button->OnClicked.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Clicked);
	TimeShop_Button->OnHovered.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Hovered);
	TimeShop_Button->OnUnhovered.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Unhovered);
	TimeShop_Button->OnPressed.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Pressed);
	TimeShop_Button->OnReleased.AddDynamic(this, &UMenu_Shop::TimeShop_Button_Realised);

	#pragma endregion

	/*Health_Img->SetBrushFromTexture(HealthShop_Upgrade_Image_Active);
	Ammo_Img->SetBrushFromTexture(AmmoShop_Upgrade_Image_Active);
	Boost_Img->SetBrushFromTexture(BoostShop_Upgrade_Image_Active);
	Time_Img->SetBrushFromTexture(TimeShop_Upgrade_Image_Active);*/

	////Button Images
	//UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	//if (GameInstance)
	//{
	//	if (GameInstance->HealthActivate == true)
	//		HealthShop_Uprade_Image->SetBrushFromTexture(HealthShop_Uprade_Image_Active);
	//	else
	//		HealthShop_Uprade_Image->SetBrushFromTexture(HealthShop_Uprade_Image_Inactive);

	//	if (GameInstance->AmmoActivate == true)
	//		AmmoShop_Uprade_Image->SetBrushFromTexture(AmmoShop_Uprade_Image_Active);
	//	else
	//		AmmoShop_Uprade_Image->SetBrushFromTexture(AmmoShop_Uprade_Image_Inactive);

	//	if (GameInstance->BoostActivate == true)
	//		BoostShop_Uprade_Image->SetBrushFromTexture(BoostShop_Uprade_Image_Active);
	//	else
	//		BoostShop_Uprade_Image->SetBrushFromTexture(BoostShop_Uprade_Image_Inactive);

	//	if (GameInstance->TimeActivate == true)
	//		TimeShop_Uprade_Image->SetBrushFromTexture(TimeShop_Uprade_Image_Active);
	//	else
	//		TimeShop_Uprade_Image->SetBrushFromTexture(TimeShop_Uprade_Image_Inactive);
	//}
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
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		Currency1_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->Currency1)));
		Currency2_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->Currency2)));
	}
}

void UMenu_Shop::Back_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BackSound, 1.f * GameInstance->GlobalVolumeMultiplier);
	}
}

void UMenu_Shop::SetHealthUpgrade_Display()
{
	#pragma region Image
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->HealthUpgradeTier <= 0)
			HealthUpgrade_Image->SetBrushFromTexture(Base);
		else if (GameInstance->HealthUpgradeTier == 1)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade1);
		else if (GameInstance->HealthUpgradeTier == 2)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade2);
		else if (GameInstance->HealthUpgradeTier == 3)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade3);
		else if (GameInstance->HealthUpgradeTier == 4)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade4);
		else if (GameInstance->HealthUpgradeTier == 5)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade5);
		else if (GameInstance->HealthUpgradeTier == 6)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade6);
		else if (GameInstance->HealthUpgradeTier == 7)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade7);
		else if (GameInstance->HealthUpgradeTier == 8)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade8);
		else if (GameInstance->HealthUpgradeTier == 9)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade9);
		else if (GameInstance->HealthUpgradeTier >= 10)
			HealthUpgrade_Image->SetBrushFromTexture(HealthUpgrade10);
	}
	#pragma endregion
	
	if (GameInstance->HealthUpgradeTier >= 10)
	{
		HealthShop_Button->SetVisibility(ESlateVisibility::Hidden);
		HealthShop_Button_Text->SetVisibility(ESlateVisibility::Hidden);
		Health_Info_Text->SetText(FText::FromString("Your Health has reached its max"));
	}

	if (GameInstance)
	{
		if (GameInstance->HealthActivate == true)
			HealthShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->HealthUpgradeCost)));
		else
			HealthShop_Button_Text->SetText(FText::FromString(""));

		/*if (GameInstance->HealthActivate == true)
			Health_Img->SetBrushFromTexture(Health_Image_Open_Idle);
		else
			Health_Img->SetBrushFromTexture(Health_Image_Locked_Idle);*/
		
		MaxHealth_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->MaxHealth)));
	}
}
void UMenu_Shop::HealthShop_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->HealthActivate == true)
		{
			if (GameInstance->HealthUpgradeTier < 10)
			{
				if (GameInstance->Currency1 >= GameInstance->HealthUpgradeCost)
				{
					if (GameInstance->HealthUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);

					GameInstance->Currency1 -= GameInstance->HealthUpgradeCost;
					GameInstance->HealthUpgradeTier += 1;
					GameInstance->HealthUpgradeCost += 1;
					GameInstance->MaxHealth += 1;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
		else
		{
			if (GameInstance)
			{
				if (GameInstance->Currency2 >= 1)
				{
					GameInstance->HealthActivate = true;
					GameInstance->Currency2 -= 1;

					UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound, 1.f * GameInstance->GlobalVolumeMultiplier);
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
	}

	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Hover);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Open_Hover);
}
void UMenu_Shop::HealthShop_Button_Hovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Hover);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Locked_Hover);
}
void UMenu_Shop::HealthShop_Button_Unhovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Idle);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Locked_Idle);
}
void UMenu_Shop::HealthShop_Button_Pressed()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Pressed);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Locked_Pressed);
}
void UMenu_Shop::HealthShop_Button_Realised()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->HealthActivate == true)
		Health_Img->SetBrushFromTexture(Health_Image_Open_Idle);
	else
		Health_Img->SetBrushFromTexture(Health_Image_Locked_Idle);
}

void UMenu_Shop::SetAmmoUpgrade_Display()
{
	#pragma region Image
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->AmmoUpgradeTier <= 0)
			AmmoUpgrade_Image->SetBrushFromTexture(Base);
		else if (GameInstance->AmmoUpgradeTier == 1)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade1);
		else if (GameInstance->AmmoUpgradeTier == 2)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade2);
		else if (GameInstance->AmmoUpgradeTier == 3)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade3);
		else if (GameInstance->AmmoUpgradeTier == 4)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade4);
		else if (GameInstance->AmmoUpgradeTier == 5)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade5);
		else if (GameInstance->AmmoUpgradeTier == 6)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade6);
		else if (GameInstance->AmmoUpgradeTier == 7)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade7);
		else if (GameInstance->AmmoUpgradeTier == 8)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade8);
		else if (GameInstance->AmmoUpgradeTier == 9)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade9);
		else if (GameInstance->AmmoUpgradeTier >= 10)
			AmmoUpgrade_Image->SetBrushFromTexture(AmmoUpgrade10);
	}
	#pragma endregion

	if (GameInstance->AmmoUpgradeTier >= 10)
	{
		AmmoShop_Button->SetVisibility(ESlateVisibility::Hidden);
		AmmoShop_Button_Text->SetVisibility(ESlateVisibility::Hidden);
		Ammo_Info_Text->SetText(FText::FromString("Your Ammo has reached its max"));
	}

	if (GameInstance)
	{
		if (GameInstance->AmmoActivate == true)
			AmmoShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->AmmoUpgradeCost)));
		else
			AmmoShop_Button_Text->SetText(FText::FromString(""));

		MaxAmmo_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->MaxAmmo)));
	}
}
void UMenu_Shop::AmmoShop_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->AmmoActivate == true)
		{
			if (GameInstance->AmmoUpgradeTier < 10)
			{
				if (GameInstance->Currency1 >= GameInstance->AmmoUpgradeCost)
				{
					if (GameInstance->AmmoUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);

					GameInstance->Currency1 -= GameInstance->AmmoUpgradeCost;
					GameInstance->AmmoUpgradeTier += 1;
					GameInstance->AmmoUpgradeCost += 1;
					GameInstance->MaxAmmo += 5;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
		else
		{
			if (GameInstance)
			{
				if (GameInstance->Currency2 >= 1)
				{
					GameInstance->AmmoActivate = true;
					GameInstance->Currency2 -= 1;

					UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound, 1.f * GameInstance->GlobalVolumeMultiplier);
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
	}
	
	if (GameInstance->AmmoActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Hover);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Hover);
}
void UMenu_Shop::AmmoShop_Button_Hovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->AmmoActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Hover);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Locked_Hover);
}
void UMenu_Shop::AmmoShop_Button_Unhovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->AmmoActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Idle);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Locked_Idle);
}
void UMenu_Shop::AmmoShop_Button_Pressed()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->AmmoActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Pressed);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Locked_Pressed);
}
void UMenu_Shop::AmmoShop_Button_Realised()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->AmmoActivate == true)
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Open_Idle);
	else
		Ammo_Img->SetBrushFromTexture(Ammo_Image_Locked_Idle);
}


void UMenu_Shop::SetBoostUpgrade_Display()
{
	#pragma region Image
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->BoostUpgradeTier <= 0)
			BoostUpgrade_Image->SetBrushFromTexture(Base);
		else if (GameInstance->BoostUpgradeTier == 1)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade1);
		else if (GameInstance->BoostUpgradeTier == 2)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade2);
		else if (GameInstance->BoostUpgradeTier == 3)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade3);
		else if (GameInstance->BoostUpgradeTier == 4)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade4);
		else if (GameInstance->BoostUpgradeTier == 5)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade5);
		else if (GameInstance->BoostUpgradeTier == 6)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade6);
		else if (GameInstance->BoostUpgradeTier == 7)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade7);
		else if (GameInstance->BoostUpgradeTier == 8)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade8);
		else if (GameInstance->BoostUpgradeTier == 9)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade9);
		else if (GameInstance->BoostUpgradeTier >= 10)
			BoostUpgrade_Image->SetBrushFromTexture(BoostUpgrade10);
	}
	#pragma endregion

	if (GameInstance->BoostUpgradeTier >= 10)
	{
		BoostShop_Button->SetVisibility(ESlateVisibility::Hidden);
		BoostShop_Button_Text->SetVisibility(ESlateVisibility::Hidden);
		Boost_Info_Text->SetText(FText::FromString("Your Boost has reached its max"));
	}

	if (GameInstance)
	{
		if (GameInstance->BoostActivate == true)
			BoostShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->BoostUpgradeCost)));
		else
			BoostShop_Button_Text->SetText(FText::FromString(""));

		MaxBoost_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_FloatToString(GameInstance->DashTimer)));
	}
}
void UMenu_Shop::BoostShop_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->BoostActivate == true)
		{
			if (GameInstance->BoostUpgradeTier < 10)
			{
				if (GameInstance->Currency1 >= GameInstance->BoostUpgradeCost)
				{
					if (GameInstance->BoostUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);

					GameInstance->Currency1 -= GameInstance->BoostUpgradeCost;
					GameInstance->BoostUpgradeTier += 1;
					GameInstance->BoostUpgradeCost += 1;
					GameInstance->DashTimer += 0.5f;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
		else
		{
			if (GameInstance)
			{
				if (GameInstance->Currency2 >= 1)
				{
					GameInstance->BoostActivate = true;
					GameInstance->Currency2 -= 1;

					UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound, 1.f * GameInstance->GlobalVolumeMultiplier);
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
	}

	if (GameInstance->BoostActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Hover);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Hover);
}
void UMenu_Shop::BoostShop_Button_Hovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->BoostActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Hover);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Locked_Hover);
}
void UMenu_Shop::BoostShop_Button_Unhovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->BoostActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Idle);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Locked_Idle);
}
void UMenu_Shop::BoostShop_Button_Pressed()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->BoostActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Pressed);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Locked_Pressed);
}
void UMenu_Shop::BoostShop_Button_Realised()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->BoostActivate == true)
		Boost_Img->SetBrushFromTexture(Boost_Image_Open_Idle);
	else
		Boost_Img->SetBrushFromTexture(Boost_Image_Locked_Idle);
}


void UMenu_Shop::SetTimeUpgrade_Display()
{
	#pragma region Image
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->TimeUpgradeTier <= 0)
			TimeUpgrade_Image->SetBrushFromTexture(Base);
		else if (GameInstance->TimeUpgradeTier == 1)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade1);
		else if (GameInstance->TimeUpgradeTier == 2)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade2);
		else if (GameInstance->TimeUpgradeTier == 3)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade3);
		else if (GameInstance->TimeUpgradeTier == 4)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade4);
		else if (GameInstance->TimeUpgradeTier == 5)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade5);
		else if (GameInstance->TimeUpgradeTier == 6)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade6);
		else if (GameInstance->TimeUpgradeTier == 7)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade7);
		else if (GameInstance->TimeUpgradeTier == 8)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade8);
		else if (GameInstance->TimeUpgradeTier == 9)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade9);
		else if (GameInstance->TimeUpgradeTier >= 10)
			TimeUpgrade_Image->SetBrushFromTexture(TimeUpgrade10);
	}
	#pragma endregion

	if (GameInstance->TimeUpgradeTier >= 10)
	{
		TimeShop_Button->SetVisibility(ESlateVisibility::Hidden);
		TimeShop_Button_Text->SetVisibility(ESlateVisibility::Hidden);
		Time_Info_Text->SetText(FText::FromString("Your Extra Time has reached its max"));
	}

	if (GameInstance)
	{
		if (GameInstance->TimeActivate == true)
			TimeShop_Button_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->TimeUpgradeCost)));
		else
			TimeShop_Button_Text->SetText(FText::FromString(""));

		MaxTime_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->TimeAdded)));
	}
}
void UMenu_Shop::TimeShop_Button_Clicked()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->TimeActivate == true)
		{
			if (GameInstance->TimeUpgradeTier < 10)
			{
				if (GameInstance->Currency1 >= GameInstance->TimeUpgradeCost)
				{
					if (GameInstance->TimeUpgradeTier == 9)
						UGameplayStatics::PlaySound2D(GetWorld(), LastUpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);
					else
						UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound, 1.f * GameInstance->GlobalVolumeMultiplier);

					GameInstance->Currency1 -= GameInstance->TimeUpgradeCost;
					GameInstance->TimeUpgradeTier += 1;
					GameInstance->TimeUpgradeCost += 1;
					GameInstance->TimeAdded += 2;
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), NotEnoughMoneySound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
		else
		{
			if (GameInstance)
			{
				if (GameInstance->Currency2 >= 1)
				{
					GameInstance->TimeActivate = true;
					GameInstance->Currency2 -= 1;

					UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound, 1.f * GameInstance->GlobalVolumeMultiplier);
				}
				else
					UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound, 1.f * GameInstance->GlobalVolumeMultiplier);
			}
		}
	}

	if (GameInstance->TimeActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Hover);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Open_Hover);
}
void UMenu_Shop::TimeShop_Button_Hovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->TimeActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Hover);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Locked_Hover);
}
void UMenu_Shop::TimeShop_Button_Unhovered()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->TimeActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Idle);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Locked_Idle);
}
void UMenu_Shop::TimeShop_Button_Pressed()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->TimeActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Pressed);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Locked_Pressed);
}
void UMenu_Shop::TimeShop_Button_Realised()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance->TimeActivate == true)
		Time_Img->SetBrushFromTexture(Time_Image_Open_Idle);
	else
		Time_Img->SetBrushFromTexture(Time_Image_Locked_Idle);
}
