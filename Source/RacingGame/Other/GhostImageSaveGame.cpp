// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostImageSaveGame.h"
#include "Kismet/KismetMathLibrary.h"

UGhostImageSaveGame::UGhostImageSaveGame()
{
	SaveName = "Unnamed Ghost";
	DateTime = *FDateTime::Now().ToString();
}
