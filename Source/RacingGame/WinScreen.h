// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WinScreen.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UWinScreen : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);

public:
	UFUNCTION()
		void Back_Button_Pressed();
	UPROPERTY(meta = (BindWidget))
		class UButton* Back_Button;


	#pragma region Timer Display
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeDisplay_Minutes_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeDisplay_Seconds_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeDisplay_Hundrets_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* _One;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* _Two;

	int Minutes{};
	int Secounds{};
	float Hundrets{};

	float InGame_Timer_Temp{};
	#pragma endregion
};
