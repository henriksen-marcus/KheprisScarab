// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu_Start.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UMenu_Start : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);

public:
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* SelectSound;

	UFUNCTION()
		void Start_Button_Clickd();
	UFUNCTION()
		void Shop_Button_Clickd();
	UFUNCTION()
		void Settings_Button_Clickd();
	UFUNCTION()
		void Quit_Button_Clickd();
	UPROPERTY(meta = (BindWidget))
		class UButton* Start_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Start_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* Shop_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Shop_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* Settings_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Settings_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* Quit_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Quit_Button_Text;
};
