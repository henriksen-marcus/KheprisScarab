// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu_Settings.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

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

	EasyDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::EasyDisplay_Button_Clicked);
	NormalDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::NormalDisplay_Button_Clicked);
	HardDisplay_Button->OnClicked.AddDynamic(this, &UMenu_Settings::HardDisplay_Button_Clicked);

	Sound_Button->OnClicked.AddDynamic(this, &UMenu_Settings::Sound_Button_Clicked);
	

	//Text on Buttons
	Back_Button_Text->SetText(FText::FromString("Back"));

}
void UMenu_Settings::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	SetBoxesSelected();
}


//------------------------ OWN FUNCTIONS ------------------------//


void UMenu_Settings::SetBoxesSelected()
{
	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		#pragma region HUD Display
		if (PlayerShip->Health_Display == true)
			HealthDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			HealthDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (PlayerShip->Ammo_Display == true)
			AmmoDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			AmmoDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (PlayerShip->Boost_Display == true)
			BoostDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			BoostDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (PlayerShip->Time_Display == true)
			TimeDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			TimeDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (PlayerShip->Currency1_Display == true)
			Currency1Display_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			Currency1Display_Button_Image->SetBrushFromTexture(Box_Unselected);

		if (PlayerShip->Currency2_Display == true)
			Currency2Display_Button_Image->SetBrushFromTexture(Box_Selected);
		else
			Currency2Display_Button_Image->SetBrushFromTexture(Box_Unselected);
		#pragma endregion

		#pragma region Difficulty
		if (PlayerShip->Difficulty_Easy == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
		}
		else if (PlayerShip->Difficulty_Normal == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
		}
		else if (PlayerShip->Difficulty_Hard == true)
		{
			EasyDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			NormalDisplay_Button_Image->SetBrushFromTexture(Box_Unselected);
			HardDisplay_Button_Image->SetBrushFromTexture(Box_Selected);
		}
		#pragma endregion

		#pragma region Sound
		if (PlayerShip->Sound == true)
		{
			Sound_Button_Image->SetBrushFromTexture(Box_Selected);
			Sound_Text->SetText(FText::FromString("On"));
		}
		else
		{
			Sound_Button_Image->SetBrushFromTexture(Box_Unselected);
			Sound_Text->SetText(FText::FromString("Off"));
		}
		#pragma endregion

	}
}

void UMenu_Settings::Back_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BackSound);
}

void UMenu_Settings::HealthDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Health_Display == true)
			PlayerShip->Health_Display = false;
		else
			PlayerShip->Health_Display = true;
	}
}
void UMenu_Settings::AmmoDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Ammo_Display == true)
			PlayerShip->Ammo_Display = false;
		else
			PlayerShip->Ammo_Display = true;
	}
}
void UMenu_Settings::BoostDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Boost_Display == true)
			PlayerShip->Boost_Display = false;
		else
			PlayerShip->Boost_Display = true;
	}
}
void UMenu_Settings::TimeDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Time_Display == true)
			PlayerShip->Time_Display = false;
		else
			PlayerShip->Time_Display = true;
	}
}
void UMenu_Settings::Currency1Display_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Currency1_Display == true)
			PlayerShip->Currency1_Display = false;
		else
			PlayerShip->Currency1_Display = true;
	}
}
void UMenu_Settings::Currency2Display_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Currency2_Display == true)
			PlayerShip->Currency2_Display = false;
		else
			PlayerShip->Currency2_Display = true;
	}
}

void UMenu_Settings::EasyDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		PlayerShip->Difficulty_Easy = true;
		PlayerShip->Difficulty_Normal = false;
		PlayerShip->Difficulty_Hard = false;

		PlayerShip->TimeCount = 75;
	}
}
void UMenu_Settings::NormalDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		PlayerShip->Difficulty_Easy = false;
		PlayerShip->Difficulty_Normal = true;
		PlayerShip->Difficulty_Hard = false;

		PlayerShip->TimeCount = 60;
	}
}
void UMenu_Settings::HardDisplay_Button_Clicked()
{
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);

	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		PlayerShip->Difficulty_Easy = false;
		PlayerShip->Difficulty_Normal = false;
		PlayerShip->Difficulty_Hard = true;

		PlayerShip->TimeCount = 45;
	}
}

void UMenu_Settings::Sound_Button_Clicked()
{
	APlayerShipPhysics* PlayerShip = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		if (PlayerShip->Sound == true)
			PlayerShip->Sound = false;
		else
			PlayerShip->Sound = true;
	}
	
	UGameplayStatics::PlaySound2D(GetWorld(), Select_Sound);
}
