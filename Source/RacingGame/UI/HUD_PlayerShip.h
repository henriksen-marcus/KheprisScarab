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
	void HighScoreDisplay();
	void LapsDisplay();
	void CheckDefeat();
	void SetGhostCheckpointTimeDisplay(float DeltaTime);

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
		class UTextBlock* Fuel_Time_Static_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Time_Text;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Fuel_Panel;
	int StartTime{};

	//Speed
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Speed_Panel;
	UPROPERTY(meta = (BindWidget))
		class UImage* Speed_Arrow;

	//HighScore
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* HighScore_Display_Panel;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HighScore_Minutes_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HighScore_Seconds_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HighScore_Hundrets_Text;

	float Timer_Temp{};
	int HighScore_Minutes{};
	int HighScore_Seconds{};
	int HighScore_Hundrets{};

	//Round Counter
	#pragma region Lap Counter
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Current_Round_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Max_Round_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Middle_Text;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Round_Counter_Panel;
	#pragma endregion

	//Checkpoint Timer Display
	#pragma region Checkpoint Timer Display
	FSlateColor GreenColor = FSlateColor(FLinearColor(0.f, 1.f, 0.f, 1.f));
	FSlateColor RedColor = FSlateColor(FLinearColor(1.f, 0.f, 0.f, 1.f));
	FSlateColor BlackColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 1.f));

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* CheckPointSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* LowOnFuel_Sound;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Checkpoint_Time_Display;
	
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


	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* GhostTime_Panel;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GhostDisplay_Minutes_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GhostDisplay_Seconds_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* GhostDisplay_Hundrets_Text;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Ghost_One;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Ghost_Two;
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Positive;
	float GhostCheckpointTimerDisplay_Timer{};
	int Ghost_Minutes{};
	int Ghost_Secounds{};
	float Ghost_Hundrets{};
	float GhostDeltaTimeDisplay{};
	bool GhostTimeDisplay_Temp{};
	int CheckpointArray_Check{};

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
		class UCanvasPanel* RealTime_Display;
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


	#pragma region Widgets
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Panel;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Currency1_Panel;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Currency2_Panel;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Boost_Panel;
	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* Ammo_Panel;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> Screen_Lose_Class;
	UPROPERTY()
		UUserWidget* Screen_Lose;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> Screen_Win_Class;
	UPROPERTY()
		UUserWidget* Screen_Win;

	bool SpawnWidget{};
	#pragma endregion

};
