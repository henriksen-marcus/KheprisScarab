// Fill out your copyright notice in the Description page of Project Settings.


#include "Global_Variables.h"

UGlobal_Variables::UGlobal_Variables()
{
	Currency1 = 100;
	Currency2 = 3;

	MaxHealth = 5.f;
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
}
