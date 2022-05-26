// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostSpline.h"
#include "Components/SplineComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGhostSpline::AGhostSpline()
{
	PrimaryActorTick.bCanEverTick = true;
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SetRootComponent(Spline);
	Spline->SetDrawDebug(true);
}

// Called when the game starts or when spawned
void AGhostSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGhostSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGhostSpline::GenerateSpline(const TArray<FVector>& Locations, const TArray<FRotator>& Rotations, const float& Time) const
{
	// Add spline points
	for (int i{}; i < Locations.Num(); i++)
	{
		Spline->AddSplinePoint(Locations[i], ESplineCoordinateSpace::World, false);
		Spline->SetRotationAtSplinePoint(i, Rotations[i], ESplineCoordinateSpace::World, false);
	}
	Spline->Duration = Locations.Num() / 60;
	Spline->UpdateSpline();
	

	/*for(int i{}; i < 500; i++)
	{
		float yoTime = FMath::Lerp(0.f,430.f,(i/500));
		DrawDebugSphere(GetWorld(), Spline->GetLocationAtTime(yoTime, ESplineCoordinateSpace::World), 50.f, 12, FColor::Blue, false, 10000);
		//DrawDebugSphere(GetWorld(), Locations[i], 50, 12, FColor::Red, false, 5000, 1, 10);
	}*/
}

