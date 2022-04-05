// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_PlayerShip.h"
#include "PlayerShip.h"
#include <Components/ProgressBar.h>

void UHUD_PlayerShip::NativeConstruct()
{
	Super::NativeConstruct();
}
void UHUD_PlayerShip::NativeOnInitialized()
{
}
void UHUD_PlayerShip::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	SetBarPercentage();
}


//-------------- OWN FUNCTIONS --------------//


void UHUD_PlayerShip::SetBarPercentage()
{
	APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (PlayerShip)
	{
		Player_Precentage = PlayerShip->CurrentHealth / PlayerShip->MaxHealth;
		PlayerHealth_Bar->SetPercent(Player_Precentage);
	}
}