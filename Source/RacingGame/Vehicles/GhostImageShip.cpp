// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostImageShip.h"
#include "Components/WidgetComponent.h"
#include "RacingGame/Global_Variables.h"
#include "../Global_Variables.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerShipPhysics.h"

AGhostImageShip::AGhostImageShip()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	Root->InitBoxExtent(FVector(200.f, 200.f, 50.f));
	SetRootComponent(Root);
	Root->SetSimulatePhysics(false);
	Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Root->SetCollisionProfileName(FName("NoCollision"));
	
	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/3DAssets/New_Ship/MasterShip.MasterShip'")).Object);
	BaseMesh->SetRelativeScale3D(FVector(.6f, .6f, .6f));
	BaseMesh->SetupAttachment(GetRootComponent());
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetCollisionProfileName(FName("NoCollision"));
	
	//BaseMesh->SetMaterial(0, ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/3DAssets/New_Ship/TMaterials/Ship_2.Ship_2'")).Object);
	//BaseMesh->SetMaterial(1, ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/3DAssets/New_Ship/TMaterials/windows_2.windows_2'")).Object);
	//BaseMesh->SetMaterial(2, ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/3DAssets/New_Ship/TMaterials/Black_2.Black_2'")).Object);

	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComp"));
	WidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
	WidgetComp->SetDrawAtDesiredSize(false);
	WidgetComp->SetupAttachment(GetRootComponent());
}

void AGhostImageShip::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<APlayerShipPhysics>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AGhostImageShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerRef && WidgetComp)
	{
		WidgetComp->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerRef->GetActorLocation()));
	}
	
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

void AGhostImageShip::PausePlayback() const
{
	GetWorld()->GetTimerManager().PauseTimer(PlayBackTimerHandle);
}

void AGhostImageShip::ResumePlayback() const
{
	GetWorld()->GetTimerManager().UnPauseTimer(PlayBackTimerHandle);
}

