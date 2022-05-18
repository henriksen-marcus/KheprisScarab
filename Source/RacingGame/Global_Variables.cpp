// Fill out your copyright notice in the Description page of Project Settings.


#include "Global_Variables.h"
#include "Vehicles/PlayerShipPhysics.h"

UGlobal_Variables::UGlobal_Variables()
{
	Currency1 = 1000;
	Currency2 = 4;

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

void UGlobal_Variables::ChangeLevel()
{
	//UGameplayStatics::OpenLevel(GetWorld(), NewLevel);
	/**
	 * The point of latent actions are that they can take more than one frame to execute and then will trigger a function when finished.
	 * Fill out the struct:
	 */

	/*UE_LOG(LogTemp, Warning, TEXT("Starting level load!"))
	
	FLatentActionInfo ActionInfo;
	ActionInfo.CallbackTarget = this;
	ActionInfo.ExecutionFunction = FName("MapLoadCompleted");
	ActionInfo.Linkage = 0;
	ActionInfo.UUID = 0;
	UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Master_Map"), true, true, ActionInfo);*/
	
}

void UGlobal_Variables::MapLoadCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Level Load Completed, mvh. GameInstance"));
}
