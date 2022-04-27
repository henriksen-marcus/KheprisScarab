// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	
	bool SaveGame(ESaveType SaveType);

	bool LoadGame(ESaveType SaveType);
	
	UPROPERTY()
	TArray<FVector> LocationArr;

	UPROPERTY()
	TArray<FRotator> RotationArr;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecording{};

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

	UPROPERTY(EditAnywhere)
	class AGhostImageShip* GhostShipRef;

	UPROPERTY(EditAnywhere)
	class APlayerShipPhysics* PlayerShipRef;

	FTimerHandle RecordingTimerHandle;

	float TimeStart{};

	UPROPERTY()
	class UGlobal_Variables* GlobalVariables;
};
