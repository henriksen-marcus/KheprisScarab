// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Global_Variables.h"
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

	UPROPERTY()
	FVector CheckpointArrowLocation{};
	
	UPROPERTY()
	FRotator CheckpointArrowRotation{};

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecording{};

	UPROPERTY(EditAnywhere)
	class AGhostImageShip* GhostShipRef;

	UPROPERTY(EditAnywhere)
	class APlayerShipPhysics* PlayerShipRef;

	FTimerHandle RecordingTimerHandle;

	FString LastSaveDate{};

	UPROPERTY()
	class UGlobal_Variables* GameInstance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGhostImageShip> GhostShipClass;

	UPROPERTY()
	TArray<FVector> LocationArr;

	UPROPERTY()
	TArray<FRotator> RotationArr;

	float RecordingTimer{};

	UFUNCTION(BlueprintCallable)
	void StartRecording();

	UFUNCTION(BlueprintCallable)
	void StopRecording();

	UFUNCTION()
	void RecordTick();

	/** Loads a ghost of x difficulty into the memory. */
	bool LoadGhost(int32 Difficulty);
	
	/** Loads and spawns a ghost of x difficulty. */
	UFUNCTION(BlueprintCallable)
	bool SpawnGhost(int32 Difficulty);

	void StartRace();

	void ChangeLevel();

	static void LogGhostSaves();
};
