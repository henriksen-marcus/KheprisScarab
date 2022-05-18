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
	
	//Set Start Time based on Difficulty
	GameInstance->TimeCount = 0;
	GameInstance->TimeCount = GameInstance->TimeStartCount;

	//Set Start Round Counter text
	Current_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->CurrentLap_Counter)));
	Max_Round_Text->SetText(FText::FromString(UKismetStringLibrary::Conv_IntToString(GameInstance->MaxLap_Counter)));
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

	RaceFinished();
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
				Time_Text->SetVisibility(ESlateVisibility::Visible);
			else
				Time_Text->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			Time_Text->SetVisibility(ESlateVisibility::Hidden);
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
			Speed_Text->SetVisibility(ESlateVisibility::Visible);
		else
			Speed_Text->SetVisibility(ESlateVisibility::Hidden);
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
				GameInstance->TimeCount = 0;
			else
				GameInstance->TimeCount -= 1;
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
			int TempSpeed = PlayerShipPhysics->Root->GetPhysicsLinearVelocity().Size() * 0.036f;
			FString Speed = UKismetStringLibrary::Conv_IntToString(TempSpeed);

			if (GameInstance->bRaceNotStarted == true)
				Speed_Text->SetText(FText::FromString("0"));
			else
				Speed_Text->SetText(FText::FromString(Speed));
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
			RealTimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Visible);
			RealTimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Visible);
			RealTimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Visible);
			Real_One->SetVisibility(ESlateVisibility::Visible);
			Real_Two->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			RealTimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Hidden);
			RealTimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Hidden);
			RealTimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Hidden);
			Real_One->SetVisibility(ESlateVisibility::Hidden);
			Real_Two->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void UHUD_PlayerShip::LapsDisplay()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		if (GameInstance->Laps_Display == true)
		{
			Current_Round_Text->SetVisibility(ESlateVisibility::Visible);
			Max_Round_Text->SetVisibility(ESlateVisibility::Visible);
			Middle_Text->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Current_Round_Text->SetVisibility(ESlateVisibility::Hidden);
			Max_Round_Text->SetVisibility(ESlateVisibility::Hidden);
			Middle_Text->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

//Remove the "!" when the countdown are inplementet
void UHUD_PlayerShip::SetTrackTimer(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

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

			#pragma region Timer text - Visible
			TimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Visible);
			TimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Visible);
			TimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Visible);
			_One->SetVisibility(ESlateVisibility::Visible);
			_Two->SetVisibility(ESlateVisibility::Visible);
			#pragma endregion

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
		TimeDisplay_Minutes_Text->SetVisibility(ESlateVisibility::Hidden);
		TimeDisplay_Seconds_Text->SetVisibility(ESlateVisibility::Hidden);
		TimeDisplay_Hundrets_Text->SetVisibility(ESlateVisibility::Hidden);
		_One->SetVisibility(ESlateVisibility::Hidden);
		_Two->SetVisibility(ESlateVisibility::Hidden);

		//Timer_SnapShot = false;
		CheckpointTimerDisplay_Timer = 0;
	}
}

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

	if (Real_Hundrets < 10)
		RealTimeDisplay_Hundrets_Text->SetText(FText::FromString("0" + UKismetStringLibrary::Conv_IntToString(Real_Hundrets)));
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

void UHUD_PlayerShip::RaceFinished()
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->CurrentLap_Counter > 3)
	{
		GameInstance->CurrentLap_Counter = 0;
		GameInstance->Racing = false;
		GameInstance->bRaceNotStarted = true;

		//Change Level
		FName NewLevel = FName("Start_Screen");
		UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
	}
}
