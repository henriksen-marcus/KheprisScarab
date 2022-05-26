// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API AMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	AMenuPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupInputComponent() override;
	
	void MouseX(float Val);
	void MouseY(float Val);
	

	void HandleMouseUpdate();

	FVector2D MouseInput;
	
	float MouseSensitivity{5.f};

	FViewportClient* Client;

	class FAnalogCursor* AnalogCursor;
	
	/*DECLARE_MULTICAST_DELEGATE(MyHyperEvent)
	void Click();*/
};
