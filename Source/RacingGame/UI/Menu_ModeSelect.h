// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu_ModeSelect.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UMenu_ModeSelect : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);
	
public:
	UFUNCTION()
		void TimeAttack_Button_Clicked();
	UFUNCTION()
		void Adventure_Button_Clicked();
	UFUNCTION()
		void Back_Button_Clicked();


	UPROPERTY(meta = (BindWidget))
		class UButton* TimeAttack_Button;
	UPROPERTY(meta = (BindWidget))
		class UButton* Adventure_Button;

	UPROPERTY(meta = (BindWidget))
		class UButton* Back_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Back_Button_Text;

	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* Select_Sound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* BackSound;
};
