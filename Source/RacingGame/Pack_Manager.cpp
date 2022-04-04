// Fill out your copyright notice in the Description page of Project Settings.


#include "Pack_Manager.h"
#include "PlayerShip.h"
#include "Components/BoxComponent.h"

// Sets default values
APack_Manager::APack_Manager()
{
	PrimaryActorTick.bCanEverTick = true;

	//--------- VARIABLES ---------//
	PackSize = 1.f;
	RotationSpeed = 1.f;


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
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APack_Manager::OnOverlapBegin);
	#pragma endregion

}

// Called when the game starts or when spawned
void APack_Manager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::PlaySound2D(GetWorld(), PackSound_Default); // Play Default Sound
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
	//Checks if the Pack collide with the SpaceShip
	if (OtherActor->IsA(APlayerShip::StaticClass()))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), PackSound_OnPlayerHit); // Play Sound

		this->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Pack_Manager - Overlapp - SUCSESS"));
	}
}

void APack_Manager::Movement(float DeltaTime)
{
	//Location - Z-axis
	FVector NewLocation = GetActorLocation();
	NewLocation.Z = NewLocation.Z + (FMath::Sin(RunningTime * 1) * 1.5f);
	SetActorLocation(NewLocation);
	RunningTime += DeltaTime;


	//Rotation
	FRotator NewRotation = GetActorRotation();
	FRotator Rotate{ 0.f, 0.3f, 0.f };
	SetActorRotation(NewRotation + Rotate);
}
