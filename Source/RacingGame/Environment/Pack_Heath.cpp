// Fill out your copyright notice in the Description page of Project Settings.


#include "Pack_Heath.h"
#include "../Vehicles/PlayerShip.h"
#include "Pack_Manager.h"
#include "Components/BoxComponent.h"

// Sets default values
APack_Heath::APack_Heath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//--------- VARIABLES ---------//
	BounceSpeed = 1.f;
	BounceHight = 1.5f;
	RotationSpeed = 0.3f;


	//-------------------------------


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

	#pragma region SETUP COLLISION
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APack_Heath::OnOverlapBegin);
	#pragma endregion
}

// Called when the game starts or when spawned
void APack_Heath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APack_Heath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Movement(DeltaTime);
}


//-------------- OWN FUNCTIONS --------------//


void APack_Heath::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Collide with SpaceShip
	if (OtherActor->IsA(APlayerShip::StaticClass()))
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (PlayerShip)
		{
			PlayerShip->CurrentHealth = PlayerShip->MaxHealth;
			UE_LOG(LogTemp, Warning, TEXT("Pack_Health | Max Health: %i"), PlayerShip->MaxHealth);
		}

		UGameplayStatics::PlaySound2D(GetWorld(), Pack_Health_HitSound); // Play Health Sound

		this->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Pack_Health - Overlapp - SUCSESS"));
	}
}

void APack_Heath::Movement(float DeltaTime)
{
	//Location - Z-axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z + (FMath::Sin(RunningTime * BounceSpeed) * BounceHight);
	SetActorLocation(NewLocation);
	RunningTime += DeltaTime;


	//Rotation
	FRotator NewRotation = GetActorRotation();
	FRotator Rotate{ 0.f, RotationSpeed, 0.f };
	SetActorRotation(NewRotation + Rotate);
}

