// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_PlayerShip.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "../RacingGameGameModeBase.h"

void UHUD_PlayerShip::NativeConstruct()
{
	Super::NativeConstruct();
}
void UHUD_PlayerShip::NativeOnInitialized()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	GameInstance->CheckPoint_Connected = false;
	GameInstance->CurrentLap_Counter = 1;
	GameInstance->PlayerCheckpointsPassed = 0;

	/*for (size_t i = 0; i < 100; i++)
	{
		GameInstance->PlayerCheckpointTime_Array[i] = 0;
		GameInstance->GhostCheckpointTime[i] = 0;
	}*/

	GameInstance->PlayerCheckpointTime_Array.Empty();
	GameInstance->PlayerCheckpointTime_Array.Init(0, 100);

	GameInstance->GhostCheckpointTime.Empty();
	GameInstance->GhostCheckpointTime.Init(0, 100);

	GameInstance->PlayerCheckpointEntered = 0;
	GameInstance->GhostCheckpointEntered = 0;
	CheckpointArray_Check = 0;
	
	//Set Start Time based on Difficulty
	GameInstance->TimeCount = 0;
	GameInstance->TimeCount = GameInstance->TimeStartCount;

	//Set Health to Max
	GameInstance->CurrentHealth = GameInstance->MaxHealth;

	//Set Ammot to Max
	GameInstance->CurrentAmmo = GameInstance->MaxAmmo;

	//Set Boost to Empty
	GameInstance->BoostPickup = false;

	//Set Display for each Mode
	if (GameInstance->TimeAttackMode)
	{
		GhostTime_Panel->SetVisibility(ESlateVisibility::Hidden);
		Currency1_Panel->SetVisibility(ESlateVisibility::Hidden);
		Currency2_Panel->SetVisibility(ESlateVisibility::Hidden);
		Boost_Panel->SetVisibility(ESlateVisibility::Hidden);
		Ammo_Panel->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->HighScoreTime <= 0)
		{
			HighScore_Display_Panel->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			HighScore_Display_Panel->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		Currency1_Panel->SetVisibility(ESlateVisibility::Visible);
		Currency2_Panel->SetVisibility(ESlateVisibility::Visible);
		Boost_Panel->SetVisibility(ESlateVisibility::Visible);
		Ammo_Panel->SetVisibility(ESlateVisibility::Visible);
		HighScore_Display_Panel->SetVisibility(ESlateVisibility::Hidden);
		GhostTime_Panel->SetVisibility(ESlateVisibility::Hidden);
	}

	//Set Start Round Counter text
	Current_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->CurrentLap_Counter)));
	Max_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->MaxLap_Counter)));

	HighScoreDisplay();
}
void UHUD_PlayerShip::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	SetHUDVisibility();

	SetBarPercentage();
	SetCurrency1_Display();
	SetCurrency2_Display();
	Boost_Display();
	SetAmmoPrecentage();
	RealTimerDisplay();

	SetTimer_Display();
	SetTimer(DeltaTime);

	SetSpeedDisplay();

	SetTrackTimer(DeltaTime);
	SetTimeDisplay_Timer(DeltaTime);
	SetRealTimeDisplay();
	LapsDisplay();

	SetlapCounter();

	SetGhostCheckpointTimeDisplay(DeltaTime);

	CheckDefeat();
	RaceFinished();
	
	//HighScoreDisplay();
}


//-------------- OWN FUNCTIONS --------------//


void UHUD_PlayerShip::SetHUDVisibility()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->TimeAttackMode)
		{
			PlayerHealth_Bar->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			if (GameInstance->Health_Display == true)
				PlayerHealth_Bar->SetVisibility(ESlateVisibility::Visible);
			else
				PlayerHealth_Bar->SetVisibility(ESlateVisibility::Hidden);
		}
		

		if (GameInstance->Ammo_Display == true)
			Ammo_Bar->SetVisibility(ESlateVisibility::Visible);
		else
			Ammo_Bar->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->Boost_Display == true)
			Boost_Image->SetVisibility(ESlateVisibility::Visible);
		else
			Boost_Image->SetVisibility(ESlateVisibility::Hidden);

		if (GameInstance->TimeAttackMode)
		{
			if (GameInstance->Time_Display == true)
				Fuel_Panel->SetVisibility(ESlateVisibility::Visible);
			else
				Fuel_Panel->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			Fuel_Panel->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GameInstance->Currency1_Display == true)
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Visible);
			Currency1_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Currency1_Image->SetVisibility(ESlateVisibility::Hidden);
			Currency1_Text->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GameInstance->Currency2_Display == true)
		{
			Currency2_Image->SetVisibility(ESlateVisibility::Visible);
			Currency2_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Currency2_Image->SetVisibility(ESlateVisibility::Hidden);
			Currency2_Text->SetVisibility(ESlateVisibility::Hidden);
		}

		if (GameInstance->Speed_Display == true)
			Speed_Panel->SetVisibility(ESlateVisibility::Visible);
		else
			Speed_Panel->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHUD_PlayerShip::SetBarPercentage()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		// Added an interpolation here to add some smoothness - Marcus
		Player_Precentage = FMath::FInterpTo(Player_Precentage, GameInstance->CurrentHealth / GameInstance->MaxHealth, GetWorld()->GetDeltaSeconds(), 8.f);
		PlayerHealth_Bar->SetPercent(Player_Precentage);
	}
}

void UHUD_PlayerShip::SetCurrency1_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		FString PlayerCurrency1 = UKismetStringLibrary::Conv_IntToString(GameInstance->Currency1);
		Currency1_Text->SetText(FText::FromString(PlayerCurrency1));
	}
}
void UHUD_PlayerShip::SetCurrency2_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		FString PlayerCurrency2 = UKismetStringLibrary::Conv_IntToString(GameInstance->Currency2);
		Currency2_Text->SetText(FText::FromString(PlayerCurrency2));
	}
}

void UHUD_PlayerShip::Boost_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->BoostPickup == true)
		{
			Boost_Image->SetBrushFromTexture(Boost_Active);
		}
		else
		{
			Boost_Image->SetBrushFromTexture(Boost_Unactive);
		}
	}
}

void UHUD_PlayerShip::SetAmmoPrecentage()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->AmmoPercentage = GameInstance->CurrentAmmo / GameInstance->MaxAmmo;
		Ammo_Bar->SetPercent(GameInstance->AmmoPercentage);
	}
}

void UHUD_PlayerShip::SetTimer_Display()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->TimeAttackMode == true)
		{
			FString Timer = UKismetStringLibrary::Conv_IntToString(GameInstance->TimeCount);
			Time_Text->SetText(FText::FromString(Timer));

			GameInstance->Time_Display = true;
			Time_Text->SetVisibility(ESlateVisibility::Visible);
			if (GameInstance->TimeCount < 10)
			{
				Fuel_Time_Static_Text->SetColorAndOpacity(RedColor);
				Time_Text->SetColorAndOpacity(RedColor);
			}
			else
			{
				Fuel_Time_Static_Text->SetColorAndOpacity(BlackColor);
				Time_Text->SetColorAndOpacity(BlackColor);
			}
		}
		else
		{
			GameInstance->Time_Display = false;
			Time_Text->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void UHUD_PlayerShip::SetTimer(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());

	if (GameInstance)
	{
		if (!GameInstance->TimeAttackMode) {return;}
		if (GameInstance->bRaceNotStarted) {return;}

		if (!GameInstance->Pause)
		{
			GameInstance->DeltaTimeCount += DeltaTime / 2;
		}

		if (GameInstance->DeltaTimeCount >= 1)
		{
			GameInstance->DeltaTimeCount = 0;

			if (GameInstance->TimeCount <= 0)
			{
				GameInstance->TimeCount = 0;
			}
			else
			{
				GameInstance->TimeCount -= 1;
				if (GameInstance->TimeCount < 10)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), LowOnFuel_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
				}
			}
		}
	}
}

void UHUD_PlayerShip::SetSpeedDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		APlayerShipPhysics* PlayerShipPhysics = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShipPhysics)
		{
			//Set angle of Speed_Arrow_Image
			Speed_Arrow->SetRenderTransformAngle(FMath::Clamp(FMath::Lerp(-135, 135, PlayerShipPhysics->Speed / 22000), -135, 135));
		}
	}
}

void UHUD_PlayerShip::RealTimerDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->RealTimer_Display == true)
		{
			RealTime_Display->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RealTime_Display->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UHUD_PlayerShip::HighScoreDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	#pragma region Setup Timer Display
	Timer_Temp = GameInstance->HighScoreTime;

	while (Timer_Temp >= 60)
	{
		UE_LOG(LogTemp, Warning, TEXT("WinScreen - Calculate Minutes"));
		Timer_Temp -= 60;
		HighScore_Minutes += 1;
	}

	while (Timer_Temp >= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("WinScreen - Calculate Seconds"));
		Timer_Temp -= 1;
		HighScore_Seconds += 1;
	}

	UE_LOG(LogTemp, Warning, TEXT("WinScreen - Add Hundreds"));
	HighScore_Hundrets = Timer_Temp;

	HighScore_Hundrets = Hundrets * 100;
	//HighScore_Hundrets -= 2;

	#pragma endregion

	#pragma region Timer Text
	if (HighScore_Minutes < 10)
		HighScore_Minutes_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(HighScore_Minutes)));
	else
		HighScore_Minutes_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(HighScore_Minutes)));

	if (HighScore_Seconds < 10)
		HighScore_Seconds_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(HighScore_Seconds)));
	else
		HighScore_Seconds_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(HighScore_Seconds)));

	if (HighScore_Hundrets < 10)
		HighScore_Hundrets_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(HighScore_Hundrets)));
	else
		HighScore_Hundrets_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(HighScore_Hundrets)));
	#pragma endregion

	#pragma region Timer text - Visible
	if (GameInstance->TimeAttackMode)
	{
		HighScore_Display_Panel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HighScore_Display_Panel->SetVisibility(ESlateVisibility::Hidden);
	}
	if (!GameInstance->HighScoreTime)
	{
		HighScore_Display_Panel->SetVisibility(ESlateVisibility::Hidden);
	}
	#pragma endregion
}

void UHUD_PlayerShip::LapsDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->Laps_Display == true)
		{
			Round_Counter_Panel->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Round_Counter_Panel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UHUD_PlayerShip::SetTrackTimer(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	GameInstance->TimerCheck = TrackTimer_Accurate;

	//Makes in-game timer counter
	if (!GameInstance->bRaceNotStarted) 
	{
		if (!GameInstance->Pause)
		{
			TrackTimer_Temp += DeltaTime;
			TrackTimer_Accurate += DeltaTime;
		}

		if (TrackTimer_Temp >= 1)
		{
			TrackTimer_Temp = 0;
			TrackTimer += 1;
		}
	}
	else
	{
		TrackTimer_Temp = 0;
		TrackTimer = 0;
	}
}
void UHUD_PlayerShip::CalculateTimeDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	GameInstance->InGame_Timer = TrackTimer_Accurate;

	int TrackTimer_TempInt = TrackTimer;
	float TrackTimer_TempAccurate = TrackTimer_Accurate;
	Minutes = 0;
	Secounds = 0;

	/*while (TrackTimer_TempInt >= 60)
	{
		TrackTimer_TempInt -= 60;
		Minutes += 1;
	}

	Secounds = TrackTimer_TempInt;*/

	while (TrackTimer_TempAccurate >= 60)
	{
		TrackTimer_TempAccurate -= 60;
		Minutes += 1;
	}

	while (TrackTimer_TempAccurate >= 1)
	{
		TrackTimer_TempAccurate -= 1;
		Secounds += 1;
	}

	Hundrets = TrackTimer_TempAccurate;
	
}
void UHUD_PlayerShip::SetTimeDisplay_Timer(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->CheckPoint_Connected)
	{
		CalculateTimeDisplay();

		if (GameInstance->NewCheckPoint == true)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), CheckPointSound, 1.f * GameInstance->GlobalVolumeMultiplier);

			Minutes_Temp = Minutes;
			Secounds_Temp = Secounds;
			Hundrets_Temp = Hundrets * 100;

			#pragma region Timer Text
			if (Minutes_Temp < 10)
				TimeDisplay_Minutes_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Minutes_Temp)));
			else
				TimeDisplay_Minutes_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Minutes_Temp)));

			if (Secounds_Temp < 10)
				TimeDisplay_Seconds_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Secounds_Temp)));
			else
				TimeDisplay_Seconds_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Secounds_Temp)));

			if (Hundrets_Temp < 10)
				TimeDisplay_Hundrets_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Hundrets_Temp)));
			else
				TimeDisplay_Hundrets_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Hundrets_Temp)));
			#pragma endregion

			Checkpoint_Time_Display->SetVisibility(ESlateVisibility::Visible);

			GameInstance->PlayerCheckpointTime_Array[GameInstance->PlayerCheckpointEntered] = TrackTimer_Accurate;

			UE_LOG(LogTemp, Warning, TEXT("Player Time: %f"), GameInstance->PlayerCheckpointTime_Array[GameInstance->PlayerCheckpointEntered]);
			UE_LOG(LogTemp, Warning, TEXT("PlayerCheckpointEntered: %d"), GameInstance->PlayerCheckpointEntered);

			GameInstance->PlayerCheckpointEntered += 1;

			GameInstance->NewCheckPoint = false;
			CheckpointTimerDisplay_Timer = 0;
		}

		CheckpointTimerDisplay_Timer += DeltaTime;

		if (CheckpointTimerDisplay_Timer >= TrackTime_DisplayTime)
		{
			CheckpointTimerDisplay_Timer = 0;
			GameInstance->CheckPoint_Connected = false;
		}
	}
	else
	{
		Checkpoint_Time_Display->SetVisibility(ESlateVisibility::Hidden);

		//Timer_SnapShot = false;
		CheckpointTimerDisplay_Timer = 0;
	}
}
//Has checkpoint variables to check

void UHUD_PlayerShip::SetRealTimeDisplay()
{
	float Real_TrackTimer_TempAccurate = TrackTimer_Accurate;
	Real_Minutes = 0;
	Real_Secounds = 0;

	while (Real_TrackTimer_TempAccurate >= 60)
	{
		Real_TrackTimer_TempAccurate -= 60;
		Real_Minutes += 1;
	}

	while (Real_TrackTimer_TempAccurate >= 1)
	{
		Real_TrackTimer_TempAccurate -= 1;
		Real_Secounds += 1;
	}

	Real_Hundrets = Real_TrackTimer_TempAccurate;
	Real_Hundrets *= 100;

	#pragma region Timer Text
	if (Real_Minutes < 10)
		RealTimeDisplay_Minutes_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Real_Minutes)));
	else
		RealTimeDisplay_Minutes_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Real_Minutes)));

	if (Real_Secounds < 10)
		RealTimeDisplay_Seconds_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Real_Secounds)));
	else
		RealTimeDisplay_Seconds_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Real_Secounds)));

	if (Real_Hundrets <= 15)
		RealTimeDisplay_Hundrets_Text->SetText(FText::FromString("00"));
	else
		RealTimeDisplay_Hundrets_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Real_Hundrets)));
	#pragma endregion
}

void UHUD_PlayerShip::SetlapCounter()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->CurrentLap_Counter > 3)
		Current_Round_Text->SetText(FText::FromString("3"));
	else
		Current_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->CurrentLap_Counter)));

	Max_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->MaxLap_Counter)));
}

void UHUD_PlayerShip::SetGhostCheckpointTimeDisplay(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->PlayerCheckpointTime_Array[CheckpointArray_Check] > 0 && GameInstance->GhostCheckpointTime[CheckpointArray_Check] > 0)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), CheckPointSound, 0.4f * GameInstance->GlobalVolumeMultiplier);

		if (GameInstance->PlayerCheckpointTime_Array[CheckpointArray_Check] > GameInstance->GhostCheckpointTime[CheckpointArray_Check])
		{
			GameInstance->PlayerAheadOfGhost = false;
			GameInstance->CheckpointTimeDifference = GameInstance->PlayerCheckpointTime_Array[CheckpointArray_Check] - GameInstance->GhostCheckpointTime[CheckpointArray_Check];
		}
		else
		{
			GameInstance->PlayerAheadOfGhost = true;
			GameInstance->CheckpointTimeDifference = GameInstance->GhostCheckpointTime[CheckpointArray_Check] - GameInstance->PlayerCheckpointTime_Array[CheckpointArray_Check];

		}

		/*if (GameInstance->PlayerCheckpointsPassed <= 0)
		{
			if (GameInstance->PlayerCheckpointTime > GameInstance->GhostCheckpointTime[0])
			{
				GameInstance->PlayerAheadOfGhost = false;
				GameInstance->CheckpointTimeDifference = GameInstance->PlayerCheckpointTime - GameInstance->GhostCheckpointTime[0];
			}
			else
			{
				GameInstance->PlayerAheadOfGhost = true;
				GameInstance->CheckpointTimeDifference = GameInstance->GhostCheckpointTime[0] - GameInstance->PlayerCheckpointTime;
			}
		}
		else
		{
			if (GameInstance->PlayerCheckpointTime > GameInstance->GhostCheckpointTime[GameInstance->PlayerCheckpointsPassed - 1])
			{
				GameInstance->PlayerAheadOfGhost = false;
				GameInstance->CheckpointTimeDifference = GameInstance->PlayerCheckpointTime - GameInstance->GhostCheckpointTime[GameInstance->PlayerCheckpointsPassed - 1];
			}
			else
			{
				GameInstance->PlayerAheadOfGhost = true;
				GameInstance->CheckpointTimeDifference = GameInstance->GhostCheckpointTime[GameInstance->PlayerCheckpointsPassed - 1] - GameInstance->PlayerCheckpointTime;
			}
		}*/

		UE_LOG(LogTemp, Warning, TEXT("Checkpoint TimeDifference: %f"), GameInstance->CheckpointTimeDifference);


		//Calculating text display
		#pragma region Calculating text display
		float GhostTrackTimer_TempAccurate = GameInstance->CheckpointTimeDifference;
		Ghost_Minutes = 0;
		Ghost_Secounds = 0;

		while (GhostTrackTimer_TempAccurate >= 60)
		{
			GhostTrackTimer_TempAccurate -= 60;
			Ghost_Minutes += 1;
		}

		while (GhostTrackTimer_TempAccurate >= 1)
		{
			GhostTrackTimer_TempAccurate -= 1;
			Ghost_Secounds += 1;
		}

		Ghost_Hundrets = GhostTrackTimer_TempAccurate;

		//Display
		Ghost_Hundrets = Ghost_Hundrets * 100;

		if (GameInstance->PlayerAheadOfGhost)
			Positive->SetText(FText::FromString("+"));
		else
			Positive->SetText(FText::FromString("-"));

		if (Ghost_Minutes < 10)
			GhostDisplay_Minutes_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Ghost_Minutes)));
		else
			GhostDisplay_Minutes_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Ghost_Minutes)));

		if (Ghost_Secounds < 10)
			GhostDisplay_Seconds_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Ghost_Secounds)));
		else
			GhostDisplay_Seconds_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Ghost_Secounds)));

		if (Ghost_Hundrets < 10)
			GhostDisplay_Hundrets_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Ghost_Hundrets)));
		else
			GhostDisplay_Hundrets_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(Ghost_Hundrets)));

		if (GameInstance->PlayerAheadOfGhost)
		{
			Positive->SetColorAndOpacity(GreenColor);
			GhostDisplay_Minutes_Text->SetColorAndOpacity(GreenColor);
			GhostDisplay_Seconds_Text->SetColorAndOpacity(GreenColor);
			GhostDisplay_Hundrets_Text->SetColorAndOpacity(GreenColor);
			Ghost_One->SetColorAndOpacity(GreenColor);
			Ghost_Two->SetColorAndOpacity(GreenColor);
		}
		else
		{
			Positive->SetColorAndOpacity(RedColor);
			GhostDisplay_Minutes_Text->SetColorAndOpacity(RedColor);
			GhostDisplay_Seconds_Text->SetColorAndOpacity(RedColor);
			GhostDisplay_Hundrets_Text->SetColorAndOpacity(RedColor);
			Ghost_One->SetColorAndOpacity(RedColor);
			Ghost_Two->SetColorAndOpacity(RedColor);
		}

		if (GameInstance->TimeAttackMode)
		{
			GhostTime_Panel->SetVisibility(ESlateVisibility::Visible);
		}
		#pragma endregion

		GhostTimeDisplay_Temp = true;

		GameInstance->PlayerCheckpointTime_Array[CheckpointArray_Check] = 0;
		GameInstance->GhostCheckpointTime[CheckpointArray_Check] = 0;
		CheckpointArray_Check += 1;
	}

	if (GhostTimeDisplay_Temp)
	{
		GhostDeltaTimeDisplay += DeltaTime;

		if (GhostDeltaTimeDisplay >= TrackTime_DisplayTime)
		{
			GhostTimeDisplay_Temp = false;
			GhostDeltaTimeDisplay = 0;
			GhostTime_Panel->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UHUD_PlayerShip::CheckDefeat()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->TimeAttackMode)
	{
		if (GameInstance->TimeCount <= 0)
		{
			if (Screen_Lose_Class)
			{
				if (!SpawnWidget)
				{
					if (!GameInstance->Pause)
					{
						GameInstance->Pause = true;

						UE_LOG(LogTemp, Warning, TEXT("HUD_PLayerShip - Spawning in Lose_Screen 1"));

						SpawnWidget = true;

						Panel->SetVisibility(ESlateVisibility::Hidden);

						Screen_Lose = CreateWidget<UUserWidget>(GetWorld(), Screen_Lose_Class);
						Screen_Lose->AddToViewport();
						Screen_Lose->SetVisibility(ESlateVisibility::Visible);
						FInputModeUIOnly InData;
						GetWorld()->GetFirstPlayerController()->SetInputMode(InData);

						APlayerShipPhysics* ShipRef = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
						if (ShipRef)
						{
							ShipRef->bHUDDisable = true;
						}
					}
				}
			}
		}
	}
	else
	{
		if (Player_Precentage <= 0)
		{
			if (Screen_Lose_Class)
			{
				if (!SpawnWidget)
				{
					if (!GameInstance->Pause)
					{
						GameInstance->Pause = true;

						UE_LOG(LogTemp, Warning, TEXT("HUD_PLayerShip - Spawning in Lose_Screen 2"));

						SpawnWidget = true;

						Panel->SetVisibility(ESlateVisibility::Hidden);

						Screen_Lose = CreateWidget<UUserWidget>(GetWorld(), Screen_Lose_Class);
						Screen_Lose->AddToViewport();
						Screen_Lose->SetVisibility(ESlateVisibility::Visible);
						FInputModeUIOnly InData;
						GetWorld()->GetFirstPlayerController()->SetInputMode(InData);

						APlayerShipPhysics* ShipRef = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
						if (ShipRef)
						{
							ShipRef->bHUDDisable = true;
						}
					}
				}
			}
		}
	}
}
void UHUD_PlayerShip::RaceFinished()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	//Open Win Widget after 3 laps
	if (GameInstance->CurrentLap_Counter > 3)
	{
		//Save Time if there is "Time Attack" Mode
		if (GameInstance->TimeAttackMode)
		{
			//Add best time to variabel GameInstance->HighScoreTime
		}

		if (Screen_Win_Class)
		{
			if (!SpawnWidget)
			{
				if (!GameInstance->Pause)
				{
					GameInstance->Pause = true;

					Panel->SetVisibility(ESlateVisibility::Collapsed);
					
					Screen_Win = CreateWidget<UUserWidget>(GetWorld(), Screen_Win_Class);
					Screen_Win->AddToViewport();
					Screen_Win->SetVisibility(ESlateVisibility::Visible);

					FInputModeUIOnly InData;
					GetWorld()->GetFirstPlayerController()->SetInputMode(InData);

					APlayerShipPhysics* ShipRef = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
					if (ShipRef)
					{
						ShipRef->bHUDDisable = true;
					}

					SpawnWidget = true;

					UE_LOG(LogTemp, Warning, TEXT("HUD_PLayerShip - Spawning in Win_Screen UUUEEE"));

					ARacingGameGameModeBase* GameModeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());
					
					if (GameModeBase)
					{
						if (TrackTimer_Accurate < GameInstance->HighScoreTime || !GameInstance->HighScoreTime)
						{
							UE_LOG(LogTemp, Warning, TEXT("Beat Highscore! Updating."))
							GameInstance->HighScoreTime = TrackTimer_Accurate;
							GameInstance->SaveGame(UGlobal_Variables::GhostImage);
						}
					}
				}
			}
		}
	}
}
