// Fill out your copyright notice in the Description page of Project Settings.


#include "Global_Variables.h"

UGlobal_Variables::UGlobal_Variables()
{
	Currency1 = 99;
	Currency2 = 3;

	MaxHealth = 200.f;
	CurrentHealth = MaxHealth;
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
	TimeCount = 60;
	TimeAdded = 15;
}

void UGlobal_Variables::BeginPlay()
{
}

void UGlobal_Variables::Tick(float DeltaTime)
{
	

	if (Sound == true)
		Music_Volum = 1;
	else
		Music_Volum = 0;
}
