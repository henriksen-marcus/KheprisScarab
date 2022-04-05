// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD_PlayerShip.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UHUD_PlayerShip : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);

	void SetBarPercentage();
public:
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* PlayerHealth_Bar;

	UPROPERTY(BlueprintReadWrite)
		float Player_CurrentHP;
	UPROPERTY(BlueprintReadWrite)
		float Player_MaxHP;
	UPROPERTY(BlueprintReadWrite)
		float Player_Precentage;
};
