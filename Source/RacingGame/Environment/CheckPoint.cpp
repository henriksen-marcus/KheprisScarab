// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoint.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../RacingGameGameModeBase.h"

// Sets default values
ACheckPoint::ACheckPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(30.f, 150.f, 80.f));
	SetRootComponent(BoxComponent);

	SpawnPointArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	SpawnPointArrow->SetupAttachment(GetRootComponent());
	SpawnPointArrow->SetRelativeLocation(FVector(0.f, 0.f, -50.f));
	
	#pragma region Mesh
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/3DAssets/Environment/AdrianCube.AdrianCube'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
	}
	#pragma endregion


}

// Called when the game starts or when spawned
void ACheckPoint::BeginPlay()
{
	Super::BeginPlay();
	
	ARacingGameGameModeBase* GamemodeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GamemodeBase)
	{
		GamemodeBase->Checkpoints.Add(this);
	}

	//Collision checker
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoint::OnOverlapBegin);
}

// Called every frame
void ACheckPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACheckPoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerShipPhysics::StaticClass()))
	{
		UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
		if (GameInstance)
		{
			if (GameInstance->PlayerCheckpointNumber == ThisCheckpointNumber - 1)
			{
				GameInstance->PlayerCheckpointNumber += 1;

				UE_LOG(LogTemp, Warning, TEXT("Checkpoint - SUCESS, No.%d"), ThisCheckpointNumber);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Checkpoint - Not in Range, No.%d"), ThisCheckpointNumber);
			}
		}
	}
}