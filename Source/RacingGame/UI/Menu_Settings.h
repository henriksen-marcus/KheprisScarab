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
	void SetBoxesSelected();

	UFUNCTION()
		void Back_Button_Clicked();

	UPROPERTY(meta = (BindWidget))
		class UButton* Back_Button;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* BackSound;

	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* Select_Sound;

	UPROPERTY(EditAnywhere, Category = "Icon")
		UTexture2D* Box_Selected;
	UPROPERTY(EditAnywhere, Category = "Icon")
		UTexture2D* Box_Unselected;

	#pragma region HUD Display
	UPROPERTY(meta = (BindWidget))
		class UButton* HealthDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* HealthDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* AmmoDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* AmmoDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* BoostDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* BoostDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* TimeDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* TimeDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* Currency1Display_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency1Display_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* Currency2Display_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* Currency2Display_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* SpeedDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* SpeedDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* RealTimeDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* RealTimeDisplay_Button_Image;

	UPROPERTY(meta = (BindWidget))
		class UButton* LapsDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* LapsDisplay_Button_Image;

	UFUNCTION()
		void HealthDisplay_Button_Clicked();
	UFUNCTION()
		void AmmoDisplay_Button_Clicked();
	UFUNCTION()
		void BoostDisplay_Button_Clicked();
	UFUNCTION()
		void TimeDisplay_Button_Clicked();
	UFUNCTION()
		void Currency1Display_Button_Clicked();
	UFUNCTION()
		void Currency2Display_Button_Clicked();
	UFUNCTION()
		void SpeedDisplay_Button_Clicked();
	UFUNCTION()
		void RealTimeDisplay_Button_Clicked();
	UFUNCTION()
		void LapsDisplay_Button_Clicked();
	#pragma endregion

	#pragma region Difficulty
	UPROPERTY(meta = (BindWidget))
		class UButton* EasyDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* EasyDisplay_Button_Image;
	UPROPERTY(meta = (BindWidget))
		class UButton* NormalDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* NormalDisplay_Button_Image;
	UPROPERTY(meta = (BindWidget))
		class UButton* HardDisplay_Button;
	UPROPERTY(meta = (BindWidget))
		class UImage* HardDisplay_Button_Image;

	UFUNCTION()
		void EasyDisplay_Button_Clicked();
	UFUNCTION()
		void NormalDisplay_Button_Clicked();
	UFUNCTION()
		void HardDisplay_Button_Clicked();
	#pragma endregion
	
	/** Added by Marcus */
	UPROPERTY(meta = (BindWidget))
	class USlider* GlobalVolumeSlider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GlobalVolumeNumber;

	/** Colorbuttons */

	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton2;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton3;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton4;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton5;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton6;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ColorButton7;
	

	UFUNCTION()
	void UpdateGlobalVolume(const float NewVolume);

	UPROPERTY()
	class UGlobal_Variables* GlobalVars; 
};


