// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Bullet.h"
#include "../Vehicles/PlayerShipPhysics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh>BulletRef(TEXT("StaticMesh'/Game/3DAssets/Other/Bullets/SM_laser_bullet.SM_laser_bullet'"));

	if (BulletRef.Succeeded())
	{
		BaseMesh->SetStaticMesh(BulletRef.Object);
	}
	//BaseMesh->SetRelativeScale3D(FVector(7.f, 7.f, 7.f));
	BaseMesh->SetCollisionProfileName(FName("OverlapAll"));
	BaseMesh->CastShadow = true;
	SetRootComponent(BaseMesh);
	

	/*TScriptDelegate<FWeakObjectPtr> StopDelegate;
	StopDelegate.BindUFunction(this, FName("Kill"));
	ProjectileComp->OnProjectileStop.Add(StopDelegate);*/
}


// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
	
	if (BulletShootSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BulletShootSound, 0.5f);
	}
	if (BulletFireFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletFireFX, GetActorLocation(), FRotator::ZeroRotator, FVector(0.3f));
	}

	// Unreal uses cm measurement, we are converting it from km/h
	BulletSpeed /= 0.036f;
}


// Called every frame
void ABullet::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	AddActorLocalOffset(FVector(BulletSpeed * DeltaTime, 0.f,0.f), true);
	
	if (TimeLived > TimeBeforeDestroy) { this->Destroy(); }
	TimeLived += DeltaTime;
}

void ABullet::SetBulletOwner(AActor* NewOwner)
{
	if (NewOwner)
	{
		BulletOwner = NewOwner;
	}
}


// ------------------------------ CUSTOM FUNCTIONS --------------------------- //

void ABullet::Kill()
{
	this->Destroy();
}


void ABullet::OnOverlapBegin (
	UPrimitiveComponent* OverlappedComponent, // Which self-owned component overlapped? (capsule component)
	AActor* OtherActor, // The other actor itself
	UPrimitiveComponent* OtherComponent, // The component of the other actor that has been collided with, or subobject
	int32 OtherbodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult ) 
{
	/* Do not collide with:
	 - Nothing
	 - Self
	 - Other instances of this class
	*/
	if (!OtherActor || OtherActor == this || !OtherComponent  || this->GetClass() == OtherActor->GetClass() || OtherActor->IsA(APlayerShipPhysics::StaticClass())) { return; }
	// Uncomment to see overlap information
	/*FString TACS = this->GetClass()->GetName();
	FString OACS = OtherActor->GetClass()->GetName();
	UE_LOG(LogTemp, Warning, TEXT("\n%s overlapped with %s\nThis class: %s\nOther Class: %s"), *this->GetName(), *OtherActor->GetName(), *TACS, *OACS);
	UE_LOG(LogTemp, Warning, TEXT("Class of other actor: %s\nClass of PlayShipPhysics %s"), *OACS, *APlayerShipPhysics::StaticClass()->GetName())*/
	UE_LOG(LogTemp, Warning, TEXT("Bullet Overlapped!"))

	// Play bullet hit sound & effect then begone
	if (BulletHitSound1 && BulletHitSound2)
	{
		if (FMath::RandBool())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound1, 0.6f, 1.6f);
		}
		else {
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound2, 0.6f, 1.6f);
		}
	}

	if (NS_BulletHitFX)
	{

		
		FRotator Rot = ((GetActorLocation() + FVector(SweepResult.Normal)) - GetActorLocation()).Rotation();
		Rot = FMath::GetReflectionVector(GetActorRotation().Vector(), SweepResult.ImpactNormal).Rotation();
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + FVector(SweepResult.Normal)*1000, FColor::Red, false, 5.f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_BulletHitFX, SweepResult.ImpactPoint, Rot, FVector(5.f));
	}
	this->Destroy();
}


