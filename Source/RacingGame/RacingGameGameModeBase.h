// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Environment/CheckPoint.h"
#include "GameFramework/GameModeBase.h"
#include "RacingGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class RACINGGAME_API ARacingGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARacingGameGameModeBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	enum ESaveType : uint8
	{
		GameState,
		GhostImage
	};
	
	UPROPERTY()
	TArray<FVector> LocationArr;

	UPROPERTY()
	TArray<FRotator> RotationArr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecording{};

	UPROPERTY(EditAnywhere)
	class AGhostImageShip* GhostShipRef;

	UPROPERTY(EditAnywhere)
	class APlayerShipPhysics* PlayerShipRef;

	FTimerHandle RecordingTimerHandle;

	float TimeStart{};

	FString LastSaveDate{};

	UPROPERTY()
	class UGlobal_Variables* GameInstance;

	UFUNCTION(BlueprintCallable)
	void StartRecording();

	UFUNCTION(BlueprintCallable)
	void StopRecording();

	UFUNCTION()
	void RecordTick();

	UFUNCTION(BlueprintCallable)
	bool SpawnGhost();

	UFUNCTION(BlueprintCallable)
	void UnloadCompleted();

	bool SaveGame(ESaveType SaveType);

	bool LoadGame(ESaveType SaveType);
};
