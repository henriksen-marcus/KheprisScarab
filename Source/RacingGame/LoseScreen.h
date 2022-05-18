// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoseScreen.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API ULoseScreen : public UUserWidget
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
};
