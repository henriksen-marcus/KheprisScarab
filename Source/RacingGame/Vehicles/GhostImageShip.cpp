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

	if (bRaceStarted)
	{
		MovementUpdate();
		CurrentTick++;
	}
	else
	{
		IdleTimer += DeltaTime;
	}

	if (IdleTimer > 5.f)
	{
		this->Destroy();
	}
}

void AGhostImageShip::MovementUpdate()
{
	if (LocationArr.IsValidIndex(CurrentTick))
	{
		SetActorLocation(LocationArr[CurrentTick]);
	}
	else
	{
		bRaceStarted = false;
	}
	
	if (RotationArr.IsValidIndex(CurrentTick))
	{
		SetActorRotation(RotationArr[CurrentTick]);
	}
	else
	{
		bRaceStarted = false;
	}
	
}

