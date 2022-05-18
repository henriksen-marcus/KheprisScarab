// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraCommon.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class RACINGGAME_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TimeBeforeDestroy{5.f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TimeLived{};

	/** How fast the bullet travels, measured in km/h. Values are automatically converted. */
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float BulletSpeed{500.f};

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletShootSound;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletHitSound1;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletHitSound2;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* BulletFireFX;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UNiagaraSystem* NS_BulletHitFX;

	UPROPERTY()
	class UGlobal_Variables* GameInstance;
	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
