// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostImageShip.h"
#include "Materials/MaterialInstanceDynamic.h"

AGhostImageShip::AGhostImageShip()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	Root->InitBoxExtent(FVector(200.f, 200.f, 50.f));
	SetRootComponent(Root);
	Root->SetSimulatePhysics(false);
	Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Root->SetCollisionProfileName(FName("NoCollision"));
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/3DAssets/Characters/Spaceship/spaceship.spaceship'")).Object);
	BaseMesh->SetRelativeScale3D(FVector(.3f, .3f, .3f));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetCollisionProfileName(FName("NoCollision"));
	BaseMesh->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/3DAssets/Characters/Spaceship/M_PlayerShip_TransLucent.M_PlayerShip_TransLucent'")).Object);

	

	
}

void AGhostImageShip::BeginPlay()
{
	Super::BeginPlay();

	// Make ghost ship translucent - didn't work
	/*UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(BaseMesh->GetMaterial(0), NULL);
	TEnumAsByte<EBlendMode> MyEnum = EBlendMode::BLEND_Translucent;
	NewMaterial->BlendMode = MyEnum;
	NewMaterial->SetScalarParameterValue(FName("OpacityVar"), 0.25f);
	NewMaterial->TwoSided = true;

	BaseMesh->SetMaterial(0, NewMaterial);*/
}

void AGhostImageShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (bPlayback) { MovementUpdate(); }
}

void AGhostImageShip::MovementUpdate()
{
	// This was an attempt to get the ship to playback in >60fps when the recording was only in 60fps.
	//SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), CurrentLocTarget, GetWorld()->GetDeltaSeconds(), 500.f) );
	SetActorLocation(CurrentLocTarget);
	
	//SetActorRotation( FMath::RInterpConstantTo(GetActorRotation(), CurrentRotTarget, GetWorld()->GetDeltaSeconds(), 100.f));
	SetActorRotation(CurrentRotTarget);
}

void AGhostImageShip::TargetPositionUpdate()
{
	if (LocationArr.IsValidIndex(CurrentTick) && RotationArr.IsValidIndex(CurrentTick))
	{
		CurrentLocTarget = LocationArr[CurrentTick];
		CurrentRotTarget = RotationArr[CurrentTick];
		CurrentTick++;
		MovementUpdate();
	}
	else
	{
		StopPlayback();
	}
}

void AGhostImageShip::StartPlayback()
{
	// Start a looping timer that continuously runs MovementUpdate(), via the PlayBackTimerHandle
	GetWorld()->GetTimerManager().SetTimer(PlayBackTimerHandle, this, &AGhostImageShip::TargetPositionUpdate, 0.0166666667f, true);
	bPlayback = true;
}

void AGhostImageShip::StopPlayback()
{
	GetWorld()->GetTimerManager().ClearTimer(PlayBackTimerHandle);
	
	FTimerHandle Handle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]{ this->Destroy(); });

	GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate, DespawnTime, false);

	bPlayback = false;
	
}

