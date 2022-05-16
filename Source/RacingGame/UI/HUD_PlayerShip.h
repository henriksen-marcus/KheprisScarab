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

	void SetHUDVisibility();
	void SetBarPercentage();
	void SetCurrency1_Display();
	void SetCurrency2_Display();
	void Boost_Display();
	void SetAmmoPrecentage();
	void SetTimer_Display();
	void SetSpeedDisplay();
	void RealTimerDisplay();
	void LapsDisplay();

	void SetTimer(float DeltaTime);

	void SetTrackTimer(float DeltaTime);
	void SetTimeDisplay_Timer(float DeltaTime);
	void CalculateTimeDisplay();

	void SetRealTimeDisplay();

	void SetlapCounter();

	void RaceFinished();

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

	//Speed
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Speed_Text;

	//Round Counter
	#pragma region Lap Counter
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Current_Round_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Max_Round_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Middle_Text;
	
	#pragma endregion

	//Checkpoint Timer Display
	#pragma region Checkpoint Timer Display
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* CheckPointSound;

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

	float CheckpointTimerDisplay_Timer{};
	int Minutes{};
	int Secounds{};
	float Hundrets{};

	bool Timer_SnapShot{};
	int Minutes_Temp{};
	int Secounds_Temp{};
	int Hundrets_Temp{};

	float TrackTimer_Temp{};
	int TrackTimer{};
	float TrackTimer_Accurate{0};
	float TrackTime_DisplayTime{2};
	#pragma endregion

	#pragma region Timer Display
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RealTimeDisplay_Minutes_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RealTimeDisplay_Seconds_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RealTimeDisplay_Hundrets_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Real_One;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Real_Two;

	int Real_Minutes{};
	int Real_Secounds{};
	float Real_Hundrets{};
	#pragma endregion


};
