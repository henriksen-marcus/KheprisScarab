// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_RaceStartCounter.h"
#include "../Global_Variables.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Texture2D.h"


void UHUD_RaceStartCounter::NativeConstruct()
{
	Super::NativeConstruct();
}
void UHUD_RaceStartCounter::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
void UHUD_RaceStartCounter::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	ReadySetGo(DeltaTime);
}


//------------------------ OWN FUNCTIONS ------------------------//


void UHUD_RaceStartCounter::ReadySetGo(float DeltaTime)
{
	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (GameInstance)
	{
		TimeCounter += DeltaTime;

		if (TimeCounter <= TimeInterval * 2 && Repetitions == 0)
		{
			GameInstance->bRaceNotStarted = true;
			GameInstance->Racing = true;
		}
		if (TimeCounter >= TimeInterval * 2 && Repetitions == 0)
		{
			TimeCounter = 0;
			Repetitions += 1;

			UGameplayStatics::PlaySound2D(GetWorld(), Wait_Sound, 0.2);

			Image_1->SetBrushFromTexture(Red);
		}
		if (TimeCounter >= TimeInterval && Repetitions == 1)
		{
			TimeCounter = 0;
			Repetitions += 1;

			UGameplayStatics::PlaySound2D(GetWorld(), Wait_Sound, 0.2);

			Image_1->SetBrushFromTexture(Red);
			Image_2->SetBrushFromTexture(Red);
		}
		if (TimeCounter >= TimeInterval && Repetitions == 2)
		{
			TimeCounter = 0;
			Repetitions += 1;

			UGameplayStatics::PlaySound2D(GetWorld(), Wait_Sound, 0.2);
			
			Image_1->SetBrushFromTexture(Yellow);
			Image_2->SetBrushFromTexture(Yellow);
			Image_3->SetBrushFromTexture(Yellow);
		}
		if (TimeCounter >= TimeInterval && Repetitions == 3)
		{
			TimeCounter = 0;
			Repetitions += 1;

			UGameplayStatics::PlaySound2D(GetWorld(), GO_Sound, 0.2);
			
			Image_1->SetBrushFromTexture(Green);
			Image_2->SetBrushFromTexture(Green);
			Image_3->SetBrushFromTexture(Green);

			GameInstance->bRaceNotStarted = false;
		}
		if (TimeCounter >= TimeInterval * 1.3f && Repetitions == 4)
		{
			TimeCounter = 0;
			Repetitions += 1;

			Image_1->SetVisibility(ESlateVisibility::Hidden);
			Image_2->SetVisibility(ESlateVisibility::Hidden);
			Image_3->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
