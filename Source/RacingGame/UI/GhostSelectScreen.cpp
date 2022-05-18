// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostSelectScreen.h"
#include "../Global_Variables.h"
#include "Menu_ModeSelect.h"

void UGhostSelectScreen::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGhostSelectScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	HighscoreBtn->OnClicked.AddDynamic(this, &UGhostSelectScreen::HighscoreBtnClicked);
	GoldBtn->OnClicked.AddDynamic(this, &UGhostSelectScreen::GoldBtnClicked);
	SilverBtn->OnClicked.AddDynamic(this, &UGhostSelectScreen::SilverBtnClicked);
	BronzeBtn->OnClicked.AddDynamic(this, &UGhostSelectScreen::BronzeBtnClicked);

	GameInstance = Cast<UGlobal_Variables>(GetWorld()->GetAuthGameMode());
	//Parent = Cast<UMenu_ModeSelect>(Parent);
}


void UGhostSelectScreen::HighscoreBtnClicked()
{
	if (GameInstance)
	{
		GameInstance->GhostDifficulty = UGlobal_Variables::EGhostDifficulty::Player;
	}
}

void UGhostSelectScreen::GoldBtnClicked()
{
	if (GameInstance)
	{
		GameInstance->GhostDifficulty = UGlobal_Variables::EGhostDifficulty::Gold;
	}
}

void UGhostSelectScreen::SilverBtnClicked()
{
	if (GameInstance)
	{
		GameInstance->GhostDifficulty = UGlobal_Variables::EGhostDifficulty::Silver;
	}
}

void UGhostSelectScreen::BronzeBtnClicked()
{
	if (GameInstance)
	{
		GameInstance->GhostDifficulty = UGlobal_Variables::EGhostDifficulty::Bronze;
	}
}
