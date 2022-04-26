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

	UPROPERTY()
		TArray<ACheckPoint*> Checkpoints;

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

	UPROPERTY()
	TArray<uint64> TickNumbers;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRecording{};

	UFUNCTION(BlueprintCallable)
	void StartRecording();

	UFUNCTION(BlueprintCallable)
	void StopRecording();

	UFUNCTION(BlueprintCallable)
	bool SpawnGhost();

	UPROPERTY(EditAnywhere)
	class AGhostImageShip* GhostShipRef;

	UPROPERTY(EditAnywhere)
	class APlayerShipPhysics* PlayerShipRef;

	FVector EndLoc;
};
