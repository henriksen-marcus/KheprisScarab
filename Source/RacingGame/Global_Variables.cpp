// Fill out your copyright notice in the Description page of Project Settings.


#include "Global_Variables.h"
#include "Vehicles/PlayerShipPhysics.h"

UGlobal_Variables::UGlobal_Variables()
{
	Currency1 = 0;
	Currency2 = 0;

	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
	TimeCount = 60;
	TimeAdded = 15;
}

void UGlobal_Variables::BeginPlay()
{
	PlayerRef = Cast<APlayerShipPhysics>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void UGlobal_Variables::Tick(float DeltaTime)
{
	

	if (Sound == true)
		Music_Volum = 1;
	else
		Music_Volum = 0;
}


void UGlobal_Variables::AddHealth(const float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);

	if (!CurrentHealth)
	{
		if (PlayerRef)
		{
			PlayerRef->Respawn();
			// Handle respawn screens and effects here, or do it in playershipphysics
			CurrentHealth = MaxHealth;
		}
	}
}
