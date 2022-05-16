// Fill out your copyright notice in the Description page of Project Settings.


#include "Pack_Manager.h"
#include "../Global_Variables.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APack_Manager::APack_Manager()
{
	PrimaryActorTick.bCanEverTick = true;

	//--------- VARIABLES ---------//
	BounceSpeed = 1.f;
	BounceHight = 0.7f;
	RotationSpeed = 0.3f;

	#pragma region BUILD PACK
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/3DAssets/Environment/AdrianCube.AdrianCube'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
	}
	#pragma endregion

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APack_Manager::OnOverlapBegin);
}

// Called when the game starts or when spawned
void APack_Manager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySound2D(GetWorld(), PackSound_Default); // Play Default Sound

	UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	if (!GameInstance) { return; }

	if (GameInstance->TimeAttackMode)
	{
		MeshComponent->SetStaticMesh(EmptyMeshComponent);
	}
}

// Called every frame
void APack_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Movement(DeltaTime);
}


//-------------- OWN FUNCTIONS --------------//


void APack_Manager::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Collide with SpaceShip
	if (OtherActor->IsA(APlayerShipPhysics::StaticClass()))
	{
		UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
		if (!GameInstance) { return; }

		if (!GameInstance->TimeAttackMode)
		{
			if (HealthPack)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), HealthPack_Sound);

					GameInstance->CurrentHealth = GameInstance->MaxHealth;
					UE_LOG(LogTemp, Warning, TEXT("Pack_Health | Max Health: %i"), GameInstance->MaxHealth);
					UE_LOG(LogTemp, Warning, TEXT("HealthPack - SUCCESS"));
				}
			}
			else if (AmmoPack)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), AmmoPack_Sound);

					GameInstance->CurrentAmmo = GameInstance->MaxAmmo;
					UE_LOG(LogTemp, Warning, TEXT("AmmoPack - SUCCESS"));
				}
			}
			else if (BoostPack)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BoostPack_Sound, 0.2f);

					GameInstance->BoostPickup = true;
					UE_LOG(LogTemp, Warning, TEXT("BoostPack - SUCCESS"));
				}
			}
			else if (Currency1)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Currency1_Sound);

					GameInstance->Currency1 += 1;
					UE_LOG(LogTemp, Warning, TEXT("Currency1 - SUCCESS | %dp."), GameInstance->Currency1);
				}
			}
			else if (Currency2)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Currency2_Sound);

					GameInstance->Currency2 += 1;
					UE_LOG(LogTemp, Warning, TEXT("Currency2 - SUCCESS | %dp."), GameInstance->Currency2);
				}
			}
			else if (TimePack)
			{
				if (GameInstance)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), Time_Sound);

					GameInstance->TimeCount += GameInstance->TimeAdded;
					UE_LOG(LogTemp, Warning, TEXT("TimePack - SUCCESS"));
				}
			}

			UGameplayStatics::PlaySound2D(GetWorld(), PackSound_OnPlayerHit); // Play Sound

			this->Destroy();
		}
	}
}

void APack_Manager::Movement(float DeltaTime)
{
	//Location - Z-axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z + (FMath::Sin(RunningTime * BounceSpeed) * BounceHight) * DeltaTime * 150;
	SetActorLocation(NewLocation);
	RunningTime += DeltaTime;

	//Rotation
	FRotator NewRotation = GetActorRotation();
	FRotator Rotate{ 0.f, RotationSpeed * DeltaTime * 200, 0.f };
	SetActorRotation(NewRotation + Rotate);
}
