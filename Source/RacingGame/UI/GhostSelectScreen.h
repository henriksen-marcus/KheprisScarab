// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GhostSelectScreen.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UGhostSelectScreen : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	
public:

	UPROPERTY(meta = (BindWidget))
	UButton* HighscoreBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* GoldBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* SilverBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* BronzeBtn;

	UFUNCTION()
	void HighscoreBtnClicked();

	UFUNCTION()
	void GoldBtnClicked();

	UFUNCTION()
	void SilverBtnClicked();

	UFUNCTION()
	void BronzeBtnClicked();

	UPROPERTY()
	class UGlobal_Variables* GameInstance;
};




