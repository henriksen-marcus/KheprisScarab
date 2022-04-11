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
	void SetCurrency1_Display();
	void SetCurrency2_Display();
	void Boost_Display();
	void SetAmmoPrecentage();
	void SetTimer_Display();
public:
	//Health Bar
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* PlayerHealth_Bar;
	UPROPERTY(BlueprintReadWrite)
		float Player_CurrentHP;
	UPROPERTY(BlueprintReadWrite)
		float Player_MaxHP;
	UPROPERTY(BlueprintReadWrite)
		float Player_Precentage;

	//Currency
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency1_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Currency1_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency2_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Currency2_Text;

	//Boost
	UPROPERTY(meta = (BindWidget))
		class UImage* Boost_Image;
	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture2D* Boost_Active;
	UPROPERTY(EditAnywhere, Category = "Image")
		UTexture2D* Boost_Unactive;

	//Ammo
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Ammo_Bar;

	//Time Attack
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Text;

};
