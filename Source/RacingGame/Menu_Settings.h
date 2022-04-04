// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu_Settings.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UMenu_Settings : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);

public:
	UFUNCTION()
		void Test_Button_Clickd();
};
