// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu_Shop.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UMenu_Shop : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);

	

public:
	void Currency_Dispaly();

	UFUNCTION()
		void Back_Button_Clicked();

	UPROPERTY(meta = (BindWidget))
		class UButton* Back_Button;

	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* ErrorSound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* UnlockSound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* UpgradeSound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* LastUpgradeSound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* NotEnoughMoneySound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* BackSound;

	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Base")
		UTexture2D* Base;


	//Currency
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency1_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Currency1_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency2_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Currency2_Text;

	//Upgade Textures
	UPROPERTY(meta = (BindWidget))
		class UImage* Health_Img;
	UPROPERTY(meta = (BindWidget))
		class UImage* Ammo_Img;
	UPROPERTY(meta = (BindWidget))
		class UImage* Boost_Img;
	UPROPERTY(meta = (BindWidget))
		class UImage* Time_Img;

	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* HealthShop_Upgrade_Image_Inactive;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* HealthShop_Upgrade_Image_Active;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* AmmoShop_Upgrade_Image_Inactive;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* AmmoShop_Upgrade_Image_Active;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* BoostShop_Upgrade_Image_Inactive;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* BoostShop_Upgrade_Image_Active;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* TimeShop_Upgrade_Image_Inactive;
	UPROPERTY(EditAnywhere, Category = "Upgrade Icon")
		UTexture2D* TimeShop_Upgrade_Image_Active;


	#pragma region Health Upgrade
	//bool HealthActivate{};

	void SetHealthUpgrade_Display();

	UFUNCTION()
		void HealthShop_Button_Clicked();
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Health_Info_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* HealthShop_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HealthShop_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* HealthUpgrade_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxHealth_Text;

	//int HealthUpgradeTier{};
	//int HealthUpgradeCost{2};

	#pragma region Upgrade Health Texture
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade1;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade2;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade3;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade4;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade5;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade6;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade7;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade8;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade9;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Health")
		UTexture2D* HealthUpgrade10;
	#pragma endregion
	#pragma endregion

	#pragma region Ammo Upgrade
	//bool AmmoActivate{};

	void SetAmmoUpgrade_Display();

	UFUNCTION()
		void AmmoShop_Button_Clicked();
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Ammo_Info_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* AmmoShop_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* AmmoShop_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* AmmoUpgrade_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxAmmo_Text;

	//int AmmoUpgradeTier{};
	//float AmmoUpgradeCost{2};

	#pragma region Upgrade Ammo Texture
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade1;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade2;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade3;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade4;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade5;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade6;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade7;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade8;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade9;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Ammo")
		UTexture2D* AmmoUpgrade10;
	#pragma endregion
	#pragma endregion

	#pragma region Boost
	//bool BoostActivate{};

	void SetBoostUpgrade_Display();

	UFUNCTION()
		void BoostShop_Button_Clicked();
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Boost_Info_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* BoostShop_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BoostShop_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* BoostUpgrade_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxBoost_Text;

	//int BoostUpgradeTier{};
	//float BoostUpgradeCost{2};

	#pragma region Upgrade Boost Texture
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade1;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade2;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade3;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade4;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade5;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade6;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade7;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade8;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade9;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Boost")
		UTexture2D* BoostUpgrade10;
	#pragma endregion
	#pragma endregion

	#pragma region Checkpoint Time
	//bool TimeActivate{};

	void SetTimeUpgrade_Display();

	UFUNCTION()
		void TimeShop_Button_Clicked();
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Info_Text;
	UPROPERTY(meta = (BindWidget))
		class UButton* TimeShop_Button;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TimeShop_Button_Text;
	UPROPERTY(meta = (BindWidget))
		class UImage* TimeUpgrade_Image;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* MaxTime_Text;

	//int TimeUpgradeTier{};
	//float TimeUpgradeCost{2};

	#pragma region Upgrade Time Texture
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade1;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade2;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade3;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade4;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade5;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade6;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade7;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade8;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade9;
	UPROPERTY(EditAnywhere, Category = "Upgrade Bar - Time")
		UTexture2D* TimeUpgrade10;
	#pragma endregion
	#pragma endregion
};
