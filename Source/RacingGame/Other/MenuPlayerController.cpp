// Fill out your copyright notInputComponente in the Description page of Project Settings.


#include "MenuPlayerController.h"
#include "Framework\Application\AnalogCursor.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Engine/UserInterfaceSettings.h"


AMenuPlayerController::AMenuPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("PlayerController BeginPlay()"))

	Client = GEngine->GameViewport->Viewport->GetClient();

	
}

void AMenuPlayerController::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMouseUpdate();
}

/**
 * The native input setup function in the base player controller class is called "SetupInputComponent()".
 * Since the UInputComponent already exists in the base class for the player controller, we don't have it
 * as an argument here, we just get it from the parent.
 */
void AMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAxis("MouseX", this, &AMenuPlayerController::MouseX);
	InputComponent->BindAxis("MouseY", this, &AMenuPlayerController::MouseY);
	//InputComponent->BindAction("SimulateMouseClick", EInputEvent::IE_Pressed,this, &AMenuPlayerController::Click);
}

void AMenuPlayerController::MouseX(const float Val)
{
	MouseInput.X = Val * MouseSensitivity;
}

void AMenuPlayerController::MouseY(const float Val)
{
	MouseInput.Y = -Val * MouseSensitivity;
}

/*
void AMenuPlayerController::Click()
{
	UE_LOG(LogTemp, Warning, TEXT("Click"))
	/*FInputKeyEventArgs MyArgs(GEngine->GameViewport->Viewport, 0, EKeys::LeftMouseButton, EInputEvent::IE_Pressed, 1, false);
	if (Client->InputKey(MyArgs))
	{
		UE_LOG(LogTemp, Warning, TEXT("true"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("false"))
	}#1#
	//Client->InputAxis(GEngine->GameViewport->Viewport, 0, )
	
	/*float x,y;
	GetMousePosition(x,y);

	TSet<FKey> MyKeys;
	MyKeys.Add(EKeys::LeftMouseButton);
	FModifierKeysState KeysState;
	FPointerEvent Event;
	
	FPointerEvent MyEvent(0, FVector2D(x,y),FVector2D(x,y), MyKeys, FKey(), 0, KeysState);
	AnalogCursor->HandleMouseButtonDownEvent(FSlateApplication::Get(), MyEvent);#1#
}
*/

void AMenuPlayerController::HandleMouseUpdate()
{
	float X, Y;
	if (GetMousePosition(X, Y))
	{
		int32 ViewportSizeX;
		int32 ViewportSizeY;
		
		GetViewportSize(ViewportSizeX, ViewportSizeY);

		// Clamp X and Y coordinates
		X = (int32)(X + MouseInput.X);
		X = X >= ViewportSizeX ? (ViewportSizeX) : X;
		X = X <= 0 ? 1 : X;

		Y = (int32)(Y + MouseInput.Y);
		Y = Y >= ViewportSizeY ? (ViewportSizeY) : Y;
		Y = Y <= 0 ? 1 : Y;
		
		SetMouseLocation(X, Y);
	}
}

