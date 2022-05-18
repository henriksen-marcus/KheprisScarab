// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUD_RaceStartCounter.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API UHUD_RaceStartCounter : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime);
	
public:
	void ReadySetGo(float DeltaTime);
	float TimeCounter{0};
	float TimeInterval{1};
	int Repetitions{0};

	UPROPERTY(meta = (BindWidget))
		class UImage* Image_1;
	UPROPERTY(meta = (BindWidget))
		class UImage* Image_2;
	UPROPERTY(meta = (BindWidget))
		class UImage* Image_3;

	UPROPERTY(EditAnywhere, Category = "Icon")
		UTexture2D* Red;
	UPROPERTY(EditAnywhere, Category = "Icon")
		UTexture2D* Yellow;
	UPROPERTY(EditAnywhere, Category = "Icon")
		UTexture2D* Green;

	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* Wait_Sound;
	UPROPERTY(EditAnywhere, Category = "Sound Effect")
		USoundBase* GO_Sound;

	UPROPERTY(EditAnywhere, Category = "Music")
		USoundBase* Track_Start;
	UPROPERTY(EditAnywhere, Category = "Music")
		USoundBase* Track_Music;
};
