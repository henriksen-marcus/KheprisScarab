// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipPhysics.h"
#include "../Global_Variables.h"
#include "../Environment/CheckPoint.h"
#include "../Other/Bullet.h"
#include "../RacingGameGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../../Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraCommon.h"
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APlayerShipPhysics::APlayerShipPhysics()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root
	{
		Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
		//Root->InitBoxExtent(FVector(1.f));
		Root->InitBoxExtent(FVector(200.f, 200.f, 50.f));
		SetRootComponent(Root);
		Root->SetSimulatePhysics(true);
		Root->SetEnableGravity(false);
		Root->SetLinearDamping(0.f);
		Root->SetAngularDamping(20.f);
		Root->SetPhysicsMaxAngularVelocityInDegrees(400.f);
		Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Root->SetCollisionProfileName(FName("Ship"));
		Root->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);
		Root->OnComponentHit.AddDynamic(this, &APlayerShipPhysics::OnHit);
	}

	// Collision
	{
		/*Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
		Collision->InitCapsuleSize(50.f, 120.f);
		Collision->SetRelativeLocation(FVector(10.f, 0.f, 0.f));
		Collision->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		Collision->SetupAttachment(GetRootComponent());
		Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Collision->SetCollisionProfileName(FName("Ship")); // Collision mot working properly? Make sure there is a collision preset named "Ship".
	
		Collision->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);
		Collision->OnComponentHit.AddDynamic(this, &APlayerShipPhysics::OnHit);*/
	}
	
	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/3DAssets/New_Ship/MasterShip.MasterShip'")).Object);
	BaseMesh->SetRelativeScale3D(FVector(.6f, .6f, .6f));
	BaseMesh->SetupAttachment(GetRootComponent());

	// SpringArm
	{
		BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BackSpringArm"));
		BackSpringArm->SetRelativeRotation(FRotator(-22.f, 0.f, 0.f));
		BackSpringArm->TargetArmLength = TargetSpringArmLength;
		BackSpringArm->bEnableCameraLag = false;
		BackSpringArm->CameraLagSpeed = 30.f; // Lower = More delay
		BackSpringArm->bEnableCameraRotationLag = true;
		BackSpringArm->CameraLagMaxDistance = 2000.f;
		BackSpringArm->CameraRotationLagSpeed = 15.f;
		BackSpringArm->bDoCollisionTest = true;
		BackSpringArm->SetupAttachment(GetRootComponent());

		BehindSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FrontSpringArm"));
		BehindSpringArm->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
		BehindSpringArm->TargetArmLength = 1500.f;
		BehindSpringArm->bEnableCameraLag = false;
		//FrontSpringArm->CameraLagSpeed = 100.f; // Lower = More delay
		BehindSpringArm->bEnableCameraRotationLag = false;
		//FrontSpringArm->CameraRotationLagSpeed = 30.f;
		BehindSpringArm->bDoCollisionTest = true;
		BehindSpringArm->SetupAttachment(GetRootComponent());

		ActiveSpringArm = BackSpringArm;
	}

	// Camera
	{
		BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BackCamera"));
		BackCamera->bUsePawnControlRotation = false;
		BackCamera->SetupAttachment(BackSpringArm, USpringArmComponent::SocketName);
		BackCamera->SetRelativeRotation(FRotator(12.f, 0.f, 0.f));

		ActiveCamera = BackCamera;

		BehindCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BehindCamera"));
		BehindCamera->bUsePawnControlRotation = false;
		BehindCamera->SetupAttachment(BehindSpringArm, USpringArmComponent::SocketName);
		BehindCamera->SetRelativeRotation(FRotator(5.f, 0.f, 0.f));
		
		FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FrontCamera"));
		FrontCamera->bUsePawnControlRotation = false;
		FrontCamera->FieldOfView = 120.f;
		FrontCamera->SetupAttachment(GetRootComponent());
		FrontCamera->SetRelativeLocation(FVector(160.f, 0.f, -20.f));

		BirdCam = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BirdCam"));
		BirdCam->SetRelativeLocation(FVector(0.f, 0.f, MinimapDistance));
		BirdCam->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
		BirdCam->FOVAngle = 50.f;
		BirdCam->SetupAttachment(GetRootComponent());
		BirdCam->TextureTarget = RenderTarget;
		BirdCam->bCaptureEveryFrame = true;
	}

	// Audio
	{
		AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
		AudioComp->SetSound(ConstructorHelpers::FObjectFinder<USoundCue>(TEXT("SoundCue'/Game/SoundEffects/EngineSounds/Running/EngineRunning2.EngineRunning2'")).Object);
		AudioComp->SetupAttachment(GetRootComponent());

		StartSound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/EngineSounds/starting-car.starting-car'")).Object;
		BoostSound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Bang/firework-boom.firework-boom'")).Object;
		HitSound1 = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Hit/impactPlate_heavy_001.impactPlate_heavy_001'")).Object;
		HitSound2 = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Hit/impactPlate_heavy_004.impactPlate_heavy_004'")).Object;
		//NewLap_Sound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Hit/impactPlate_heavy_004.impactPlate_heavy_004'")).Object;
		//RaceWon_Sound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Hit/impactPlate_heavy_004.impactPlate_heavy_004'")).Object;
	}
	
	// Location placeholders
	{
		Thrust1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust1"));
		Thrust2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust2"));
		Thrust3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust3"));
		Thrust4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust4"));

		Thrust1->SetupAttachment(GetRootComponent());
		Thrust2->SetupAttachment(GetRootComponent());
		Thrust3->SetupAttachment(GetRootComponent());
		Thrust4->SetupAttachment(GetRootComponent());

		Thrust1->SetRelativeLocationAndRotation(FVector(300.f, -100.f, 0.f), FRotator(-90.f, 90.f, 0.f));
		Thrust2->SetRelativeLocationAndRotation(FVector(300.f, 100.f, 0.f), FRotator(-90.f, -90.f, 0.f));
		Thrust3->SetRelativeLocationAndRotation(FVector(-300.f, -100.f, 0.f), FRotator(-90.f, 90.f, 0.f));
		Thrust4->SetRelativeLocationAndRotation(FVector(-300.f, 100.f, 0.f), FRotator(-90.f, -90.f, 0.f));

		BulletSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BulletSpawnPoint"));
		BulletSpawnPoint->SetupAttachment(GetRootComponent());
		BulletSpawnPoint->SetRelativeLocation(FVector(200.f, 0.f, -50.f));
	}

	// Curves
	{
		CustomCurve1 = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("CurveFloat'/Game/Misc/Curves/CustomCurve1.CustomCurve1'")).Object;
		if (!CustomCurve1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not find CustomCurve1."));
		}
		CustomCurve2 = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("CurveFloat'/Game/Misc/Curves/CustomCurve2.CustomCurve2'")).Object;
		if (!CustomCurve2)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not find CustomCurve2."));
		}
		HoverForceCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("CurveFloat'/Game/Misc/Curves/HoverForceCurve.HoverForceCurve'")).Object;
		if (!HoverForceCurve)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not find HoverForceCurve"));
		}
		MinusHoverForceCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("CurveFloat'/Game/Misc/Curves/MinusHoverForceCurve.MinusHoverForceCurve'")).Object;
		if (!MinusHoverForceCurve)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not find MinusHoverForceCurve"));
		}
	}
}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	
	InitialLocation = GetActorLocation();
	InitialTargetHeight = TargetHeight;
	InitialBackSpringArmRotation = BackSpringArm->GetRelativeRotation();
	InitialBehindSpringArmRotation = BehindSpringArm->GetRelativeRotation();
	AudioComp->Deactivate();

	BehindCamera->SetActive(false);
	FrontCamera->SetActive(false);

	if (StartSound)
	{
		if (GameInstance)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), StartSound, 1.1f * GameInstance->GlobalVolumeMultiplier);
		}

		FTimerHandle TH_BeginPlay;
		FTimerDelegate TD_BeginPlay;
		// Lambda expression
		TD_BeginPlay.BindLambda([&]{ AudioComp->FadeIn(0.8f); AudioComp->PitchMultiplier = 1.f; });
		GetWorld()->GetTimerManager().SetTimer(TH_BeginPlay, TD_BeginPlay, 0.7f, false);
	}
	else
	{
		AudioComp->Activate();
	}

	ShipWeight = Root->GetMass();
	Gravity = ShipWeight * 9.81f * 100 * 4 * GravityScalar;

	// Create and add the off road screen effect to the screen, ready for use
	if (OffTrackScreenClass)
	{
		OffTrackScreen = CreateWidget<UUserWidget>(GetWorld(), OffTrackScreenClass);
		OffTrackScreen->AddToViewport();
		OffTrackScreen->SetVisibility(ESlateVisibility::Hidden);
	}

	GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	GamemodeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());

	//Checkpoint
	InitialLocation = GetActorLocation();
	if (GameInstance->TimeAttackMode)
	{
		GameInstance->PlayerCheckpointNumber = CheckPointsAmount;
	}
	else
	{
		GameInstance->PlayerCheckpointNumber = 0;
	}
}


void APlayerShipPhysics::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MovementUpdate();

	if (bIsJumping)
	{
		Root->AddForce(GetActorUpVector() * Gravity * 15);
	}

	// Engine dynamic audio
	Speed = Root->GetPhysicsLinearVelocity().Size();

	if (bLogSpeed)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Speed is: %f"), Speed);
	}

	if (AudioComp && CustomCurve2 && GameInstance)
	{
		// Interp to smooth out speed/audio fluctuations
		AudioComp->PitchMultiplier = FMath::FInterpTo(AudioComp->PitchMultiplier, (CustomCurve2->GetFloatValue(Speed/20000.f) + 1), DeltaTime, 1.5f);
		AudioComp->SetPitchMultiplier(AudioComp->PitchMultiplier);
		AudioComp->VolumeMultiplier = AudioComp->VolumeMultiplier * GameInstance->GlobalVolumeMultiplier;
	}

	// Turning
	AddActorLocalRotation(FRotator(0.f, YawMove * DeltaTime * 110.f, 0.f));

	// Cosmetic mesh rotation
	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));

	CameraUpdate();
	
	FVector HitLoc;
	CurrentSurface = CheckSurface(HitLoc);
	
	// Sand effect
	if (CurrentSurface == "")
	{
		SpawnSandEffect(HitLoc);
		if (SandSystemEndPtr)
		{
			SandSystemEndPtr->DestroyInstance();
			SandSystemEndPtr = nullptr;
			SandEndSystemTimer = 0.f;
		}
	}
	else 
	{
		if (SandSystemPtr)
		{
			SandSystemPtr->DestroyInstance();
			SandSystemPtr = nullptr;
		
			SpawnSandEffectEnd(HitLoc);
		}
		else if (SandSystemEndPtr)
		{
			if (SandEndSystemTimer > 1.5f)
			{
				SandSystemEndPtr->DestroyInstance();
				SandSystemEndPtr = nullptr;
				SandEndSystemTimer = 0.f;
			} else
			{
				SpawnSandEffectEnd(HitLoc);
				SandEndSystemTimer += DeltaTime;
			}
		}
	}
	
	
	// If we are driving off-road
	if (CurrentSurface == "DefaultPhysicalMaterial" || CurrentSurface == "PM_Road")
	{
		bIsOnRoad = true;
		OffTrackTimer = 0.f;
		SpeedMultiplier = 1.f;
		if (OffTrackScreen)
		{
			OffTrackScreen->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		OffTrackTimer += DeltaTime;
		
		if (OffTrackTimer >= 0.5f)
		{
			bIsOnRoad = false;
			SpeedMultiplier = 0.3f;
			GameInstance->AddHealth(-10 * DeltaTime);
			if (Root->GetPhysicsLinearVelocity().Size() > 5000.f)
			{
				UGameplayStatics::PlayWorldCameraShake(GetWorld(), CamShake, ActiveCamera->GetComponentLocation(), 0, 0);
			}

			if (OffTrackScreen)
			{
				OffTrackScreen->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}

	CameraCenteringTimer += DeltaTime;
	ShootTimer += DeltaTime;
	JumpTimer += DeltaTime;
	HitSoundCooldown += DeltaTime;
}


void APlayerShipPhysics::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &APlayerShipPhysics::Forward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShipPhysics::Turn);

	PlayerInputComponent->BindAxis("MouseY", this, &APlayerShipPhysics::CameraPitch);
	PlayerInputComponent->BindAxis("MouseX", this, &APlayerShipPhysics::CameraYaw);

	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed,this, &APlayerShipPhysics::Shoot);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Jump);
	PlayerInputComponent->BindAction("UseItem", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Dash);

	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Crouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &APlayerShipPhysics::CrouchEnd);

	PlayerInputComponent->BindAction("LookBehind", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::LookBehind);
	PlayerInputComponent->BindAction("LookBehind", EInputEvent::IE_Released, this, &APlayerShipPhysics::LookBehind);

	PlayerInputComponent->BindAction("ChangeCameraAngle", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::ChangeCameraAngle);

	PlayerInputComponent->BindAction("Respawn", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Respawn);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //


void APlayerShipPhysics::Forward(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		// Determine if there is input
		const bool bForwardHasInput = !(Value == 0);

		bIsBraking = bForwardHasInput ? (Value < 0.f ? true : false) : false;

		// If there is input, set rotation target to based on input value, else set target to 0
		const float TargetPitch = bForwardHasInput ? (Value > 0.f ? -5.0f : 5.f) : 0.f;

		// Interpolate rotation towards target
		if (BaseMesh)
		{
			NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 5.0f);
		}

		if (Value)
		{
			Force.X = FMath::FInterpTo(Force.X, (Value * 2.f * SpeedBoost * SpeedMultiplier * ForwardsSpeed), GetWorld()->GetDeltaSeconds(), 1.5f);
		}
		else
		{
			Force.X = 0.f;
		}
	}
}

void APlayerShipPhysics::Turn(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		// Determine if there is input
		bool bRollHasInput = !(Value == 0);

		// Roll
		float TargetRoll = bRollHasInput ? Value > 0 ? 5.f : -5.f : 0.f;

		// Interpolate rotation towards target
		if (BaseMesh)
		{
			NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);
		}
		
		// Yaw
		YawMove = FMath::FInterpTo(YawMove, Value * 2.2f, GetWorld()->GetDeltaSeconds(), 5.f);
	}
}

void APlayerShipPhysics::CameraYaw(const float Value)
{
	if (Value) { CameraCenteringTimer = 0.f; }
	SpringArmLocalChange.Yaw = Value/2;
}

void APlayerShipPhysics::CameraPitch(const float Value)
{
	if (Value) { CameraCenteringTimer = 0.f; }
	SpringArmLocalChange.Pitch = Value;
}

void APlayerShipPhysics::CameraUpdate()
{
	/** Springarm rotation */
	if (CameraCenteringTimer >= CameraResetTime)
	{
		// Reset rotation
		if (ActiveSpringArm == BackSpringArm)
		{
			ActiveSpringArm->SetRelativeRotation(FMath::RInterpTo(ActiveSpringArm->GetRelativeRotation(), InitialBackSpringArmRotation, GetWorld()->GetDeltaSeconds(), 6.f));
		}
		else
		{
			ActiveSpringArm->SetRelativeRotation(FMath::RInterpTo(ActiveSpringArm->GetRelativeRotation(), InitialBehindSpringArmRotation, GetWorld()->GetDeltaSeconds(), 6.f));
		}
	}
	else
	{
		float NewPitch = ActiveSpringArm->GetRelativeRotation().Pitch + SpringArmLocalChange.Pitch;
		SpringArmLocalChange.Pitch = NewPitch > 30	?	 30 - ActiveSpringArm->GetRelativeRotation().Pitch : SpringArmLocalChange.Pitch; // MAX
		SpringArmLocalChange.Pitch = NewPitch < -80	?	-80 - ActiveSpringArm->GetRelativeRotation().Pitch : SpringArmLocalChange.Pitch; // MIN

		float NewYaw = ActiveSpringArm->GetRelativeRotation().Yaw + SpringArmLocalChange.Yaw;
		SpringArmLocalChange.Yaw = NewYaw > 180		?	 180 - ActiveSpringArm->GetRelativeRotation().Yaw : SpringArmLocalChange.Yaw; // MAX
		SpringArmLocalChange.Yaw = NewYaw < -180	?	-180 - ActiveSpringArm->GetRelativeRotation().Yaw : SpringArmLocalChange.Yaw; // MIN
		
		/*if(NewPitch > 30)
		{
			SpringArmLocalChange.Pitch = 30 - ActiveSpringArm->GetRelativeRotation().Pitch;
		}
		else if (NewPitch < -80)
		{
			SpringArmLocalChange.Pitch = -80 - ActiveSpringArm->GetRelativeRotation().Pitch;
		}*/
		ActiveSpringArm->AddRelativeRotation(SpringArmLocalChange);
		//FRotator RelRot = ActiveSpringArm->GetRelativeRotation();
		//RelRot.Pitch = FMath::Clamp(ActiveSpringArm->GetRelativeRotation().Pitch, -80.f, 30.f);
		//ActiveSpringArm->SetRelativeRotation(RelRot);
	}

	FRotator CurRot = ActiveSpringArm->GetRelativeRotation();
	CurRot.Roll = 0;
	ActiveSpringArm->SetRelativeRotation(CurRot);

	/** Camera effects */
	BackCamera->SetFieldOfView(FMath::FInterpTo(BackCamera->FieldOfView, TargetCameraFOV, GetWorld()->GetDeltaSeconds(), 5.f));
	BackSpringArm->TargetArmLength = FMath::FInterpTo(BackSpringArm->TargetArmLength, TargetSpringArmLength, GetWorld()->GetDeltaSeconds(), 10.f);

	// Minimap camera
	BirdCam->SetWorldLocation(GetActorLocation() + FVector::UpVector * MinimapDistance);
	BirdCam->SetWorldRotation(FRotator(-90.f, GetActorRotation().Yaw, 0.f));
}

FString APlayerShipPhysics::CheckSurface(FVector &HitLocation)
{
	/** How far to check for underlying surface */
	static float CheckDistance = InitialTargetHeight * 2 + 500;
	
	FHitResult HitRes;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - GetActorUpVector() * CheckDistance;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	
	if (GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_Visibility))
	{
		if (HitRes.bBlockingHit)
		{
			HitLocation = HitRes.ImpactPoint;
			
			if (IsValid(HitRes.Component.Get()))
			{
				if (IsValid(HitRes.Component.Get()->GetMaterial(0)))
				{
					if (IsValid(HitRes.Component.Get()->GetMaterial(0)->GetPhysicalMaterial()))
					{
						UPhysicalMaterial* PhysMat = HitRes.Component.Get()->GetMaterial(0)->GetPhysicalMaterial();
						if (PhysMat)
						{
							return PhysMat->GetName();
						}
					}
				}
			}
		}
	}
	return FString();
}

void APlayerShipPhysics::SpawnSandEffect(FVector HitLoc)
{
	if (!SandSystemPtr)
	{
		if (NS_SandSystem)
		{
			SandSystemPtr = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_SandSystem, HitLoc, FRotator::ZeroRotator);
		}
	}
	else
	{
		SandSystemPtr->SetWorldLocation(HitLoc);
		SandSystemPtr->SetWorldRotation(GetActorRotation());
	}
}

void APlayerShipPhysics::SpawnSandEffectEnd(FVector HitLoc)
{
	if (!SandSystemEndPtr)
	{
		if (NS_SandSystem_End)
		{
			SandSystemEndPtr = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_SandSystem_End, HitLoc, FRotator::ZeroRotator);
			SandSystemEndPtr->AdvanceSimulationByTime(2.68f, GetWorld()->GetDeltaSeconds());
		}
	}
	else
	{
		SandSystemEndPtr->SetWorldLocation(HitLoc);
		SandSystemEndPtr->SetWorldRotation(GetActorRotation());
	}
}

void APlayerShipPhysics::Shoot()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }

		ShootTimer = 0.f;
		if (GameInstance)
		{
			if (GameInstance->CurrentAmmo > 0) {
				--GameInstance->CurrentAmmo;
				if (GetWorld())
				{
					// Spawn bullet
					GetWorld()->SpawnActor<ABullet>(BulletClassToSpawn, BulletSpawnPoint->GetComponentLocation(), GetActorRotation());
					//Root->AddImpulse(-GetActorForwardVector() * 100000.f);
				}
			}
		}
		else if (GunClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GunClickSound, 0.8f  * GameInstance->GlobalVolumeMultiplier);
		}
	}
}

void APlayerShipPhysics::DashNotifyEvent_Implementation()
{
}

void APlayerShipPhysics::Dash()
{
if (bIsDashing) { return; }

	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		if (GameInstance->BoostPickup)
		{
			GameInstance->BoostPickup = false;
		
			static float CamFovChange = 15.f;
			static float SpringArmChange = 200.f;

			TargetCameraFOV += CamFovChange;
			TargetSpringArmLength -= SpringArmChange;
			BackSpringArm->CameraLagSpeed = 35.f;
			BackSpringArm->CameraRotationLagSpeed = 20.f;
			SpeedBoost = MaxSpeedBoost;
			bIsDashing = true;

			if (BoostSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BoostSound, 0.5f * GameInstance->GlobalVolumeMultiplier);
			}

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			// Lambda expression
			TimerDelegate.BindLambda([&]
				{
					TargetCameraFOV -= CamFovChange;
					TargetSpringArmLength += SpringArmChange;
					BackSpringArm->CameraLagSpeed = 30.f;
					BackSpringArm->CameraRotationLagSpeed = 15.f;
					SpeedBoost = 1.f;
					bIsDashing = false;
				});
		
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, GameInstance->DashTimer, false);
			
			DashNotifyEvent();
		}
	}
}

void APlayerShipPhysics::Jump()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted || bIsJumping || JumpTimer < 2.f) { return; }

		bIsJumping = true;
		JumpTimer = 0.f;

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;

		// Lambda expression
		TimerDelegate.BindLambda([&]
			{
				bIsJumping = false;
			});

		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.07f, false);
	}
}

void APlayerShipPhysics::Respawn()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }

		if (CheckPoint_Last)
		{
			if (GameInstance->TimeAttackMode)
			{
				SetActorLocation(CheckPoint_Last->SpawnPointArrowTimeAttack->GetComponentLocation());
				SetActorRotation(CheckPoint_Last->SpawnPointArrowTimeAttack->GetComponentRotation());
			}
			else
			{
				SetActorLocation(CheckPoint_Last->SpawnPointArrow->GetComponentLocation());
				SetActorRotation(CheckPoint_Last->SpawnPointArrow->GetComponentRotation());
			}
			

			UE_LOG(LogTemp, Warning, TEXT("Respawn %d - Success"), CheckPoint_Last->ThisCheckpointNumber);
		}
		else
		{
			SetActorLocation(InitialLocation);

			UE_LOG(LogTemp, Warning, TEXT("Respawn - Failed"));
		}
	}
}

void APlayerShipPhysics::Crouch()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		TargetHeight /= 2.f;
	}
}

void APlayerShipPhysics::CrouchEnd()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		TargetHeight *= 2.f;
	}
}

void APlayerShipPhysics::ChangeCameraAngle()
{
	// Don't trigger if the player is looking behind
	if (BehindCamera->IsActive()) { return; }
	
	switch (++CurrentCameraAngle)
	{
	case Close:
		FrontCamera->SetActive(false);
		BackCamera->SetActive(true);
		ActiveCamera = BackCamera;
		
		TargetSpringArmLength = 700.f;
		break;
	case Far:
		FrontCamera->SetActive(false);
		BackCamera->SetActive(true);
		ActiveCamera = BackCamera;
		
		TargetSpringArmLength = 1200.f;
		break;
	case Front:
		BackCamera->SetActive(false);
		FrontCamera->SetActive(true);
		ActiveCamera = FrontCamera;

		TargetSpringArmLength = 700.f;
		BackSpringArm->TargetArmLength = 700.f;
		break;
	case END_ENUM:
		CurrentCameraAngle = -1;
		ChangeCameraAngle();
		break;
	}
}

/*void APlayerShipPhysics::SloMo(float Amount, float Duration)
{
	if (bIsInSloMo) { return; }
	
	FTimerHandle Handle;
	FTimerDelegate TimerDelegate1, TimerDelegate2;

	// This lambda function will loop to ensure the interpolation works
	TimerDelegate1.BindLambda([&]
		{
			float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
			float NewDilation = FMath::FInterpTo(CurrentDilation, Amount, GetWorld()->GetDeltaSeconds(), 5.f);
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);
		});

	// This lambda function will stop the loop after the given duration
	TimerDelegate2.BindLambda([&]
		{
			GetWorld()->GetTimerManager().ClearTimer(TimeDilationHandle);
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
			bIsInSloMo = false;
			UE_LOG(LogTemp, Warning, TEXT("TIME RESET!"))
		});

	GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, TimerDelegate1, 0.0167f, true);
	GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate2, Duration, false);
	bIsInSloMo = true;
}*/

void APlayerShipPhysics::LookBehind()
{
	// Swap active camera based on which is active.
	if (BehindCamera->IsActive())
	{
		BehindCamera->SetActive(false);
		ActiveCamera->SetActive(true);
		if (ActiveCamera == BackCamera)
		{
			ActiveSpringArm = BackSpringArm;
			BackSpringArm->SetRelativeRotation(InitialBackSpringArmRotation);
		}
	}
	else
	{
		BackCamera->SetActive(false);
		FrontCamera->SetActive(false);
		BehindCamera->SetActive(true);
		ActiveSpringArm = BehindSpringArm;
		BehindSpringArm->SetRelativeRotation(InitialBehindSpringArmRotation);
	}
}

void APlayerShipPhysics::MovementUpdate()
{
	// Local + Global gravity, added a little more force the the opposing (local) force
	const FVector CombinedGravity = ( FVector::DownVector - (GetActorUpVector() * 1.1f) ).GetSafeNormal();
	Root->AddForce(CombinedGravity * (Gravity));
	//Root->AddForce(Gravity * FVector::DownVector);

	// Local forwards force
	Root->AddForce(GetActorForwardVector() * Force.X, FName(), true);
	
	// Limit speed
	if (Root->GetPhysicsLinearVelocity().Size() > 15000.f)
	{
		if (bIsDashing && Root->GetPhysicsLinearVelocity().Size() > 22000.f)
		{
			Root->SetPhysicsLinearVelocity(Root->GetPhysicsLinearVelocity().GetSafeNormal() * 22000.f);
		}
		Root->SetPhysicsLinearVelocity(Root->GetPhysicsLinearVelocity().GetSafeNormal() * 15000.f);
	}

	// Drag
	FVector LinearVelocity = Root->GetPhysicsLinearVelocity();
	LinearVelocity.Z = 0.f;
	Root->AddForce(-LinearVelocity * 0.85f, FName(), true);

	// Makes the ship go more in it's forward direction, instead of following inertia
	Root->SetPhysicsLinearVelocity(FMath::VInterpTo(Root->GetPhysicsLinearVelocity(), GetActorForwardVector() * Root->GetPhysicsLinearVelocity().Size(), GetWorld()->GetDeltaSeconds(), 0.92f));

	HoverRaycast();
}

void APlayerShipPhysics::HoverRaycast()
{
	float RayCastLength = InitialTargetHeight * 2 + 500;
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);

	// Used to determine if we are in the air - Counter == 4 -> Four raycast misses
	int32 Counter{};

	// Start raycast
	for (int32 i{}; i < 4; i++)
	{
		FVector Start, End;

		switch (i)
		{
		case 0:
			Start = Thrust1->GetComponentLocation();
			End = Start + Thrust1->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 1:
			Start = Thrust2->GetComponentLocation();
			End = Start + Thrust2->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 2:
			Start = Thrust3->GetComponentLocation();
			End = Start + Thrust3->GetComponentRotation().Vector() * RayCastLength;
			break;
		case 3:
			Start = Thrust4->GetComponentLocation();
			End = Start + Thrust4->GetComponentRotation().Vector() * RayCastLength;
			break;
		default:
			//UE_LOG(LogTemp, Warning, TEXT("Error in APlayerShipPhysics::MovementUpdate()"))
			break;
		}

		FCollisionQueryParams CollisionParams;

		if (bEnableDebugLines)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1.f, 0, 5.f);
		}

		// Raycast
		if (GetWorld()->LineTraceSingleByChannel(HitPoints[i], Start, End, ECC_Visibility, CollisionParams))
		{
			if (bEnableDebugLines)
			{
				// Sphere at collision point
				DrawDebugSphere(GetWorld(), HitPoints[i].Location, 60.f, 12, FColor::Blue);
			}
		}
		else
		{
			// If the raycast didn't hit anything within the raycast length
			Counter++;
		}
		
	}

	// Raycast for ground sticking
	if (bIsOnRoad) {
		FHitResult HitRes;
		FVector Start = GetActorLocation();
		FVector End = GetActorLocation() - GetActorUpVector() * RayCastLength;
		
		if (GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_Visibility))
		{
			// The distance from the ship to to ground, normalized (1 is the target height)
			float NormalizedDistance = UKismetMathLibrary::Vector_Distance(Start, HitRes.Location) / TargetHeight;

			// If we are above our target hovering distance, add a downwards force to keep us on the track
			if (NormalizedDistance > 1.f)
			{
				FVector UpVector = GetActorUpVector();
				float Constant = Gravity / 2.f;
				float ZVelocity = FVector::DotProduct(Root->GetPhysicsLinearVelocity(), GetActorUpVector()); // Note: This value will be negative!
				
				const FVector ThrustForce = (Constant * HoverForceCurve->GetFloatValue(FMath::Clamp(2.5f - NormalizedDistance, 0.5f, 2.5f)) * -UpVector);
				const FVector Damping = ZVelocity * UpVector * (CustomCurve2->GetFloatValue(NormalizedDistance) + 1) * 1000;
				
				Root->AddForce(ThrustForce);
				Root->AddForce(Damping);
				
				UE_LOG(LogTemp, Warning, TEXT("DownSpeed: %f"), FVector::DotProduct(Root->GetPhysicsLinearVelocity(), GetActorUpVector()))
			}
		}
	}
	
	// Decide what to do based on how many raycasts were hits
	for (int i{}; i < HitPoints.Num(); i++)
	{
		// If the raycast was not a hit, do nothing
		if (!HitPoints[i].bBlockingHit) { continue; }
		
		switch (i)
		{
		case 0:
			AddForce(HitPoints[0].Location, 1);
			break;
		case 1:
			AddForce(HitPoints[1].Location, 2);
			break;
		case 2:
			AddForce(HitPoints[2].Location, 3);
			break;
		case 3:
			AddForce(HitPoints[3].Location, 4);
			break;
		default:
			break;
		}
	}
	
	// If we are in the air
	if (Counter == 4)
	{
		// Interpolate ship back to upright position if we are in the air
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, -10.f, GetWorld()->GetDeltaSeconds(), 0.6f);
		CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 1.f);
		SetActorRotation(CurrentRotation);
		Root->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	}
}

/*
void APlayerShipPhysics::RaycastHover()
{
	FHitResult HitRes;
	FCollisionQueryParams Params;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - GetActorUpVector() * (TargetHeight + 500);
	FVector ThrustForce = FVector::ZeroVector;

	FVector UpVector = GetActorUpVector();
	FVector GravityForceVector = FVector::DownVector * 1500;
	float HoverForceReduction = 80; // S
	float HoverDampingFactor = 2.f; // K


	if (GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECollisionChannel::ECC_Visibility, Params))
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red);
		DrawDebugSphere(GetWorld(), HitRes.Location + UpVector * 800.f, 30.f, 16, FColor::Orange);
		DrawDebugSphere(GetWorld(), HitRes.Location + UpVector * 1200.f, 30.f, 16, FColor::Orange);

		float Distance = UKismetMathLibrary::Vector_Distance(Start, HitRes.Location);
		float ScaleHeight = (TargetHeight - Distance) / HoverForceReduction;
		FVector HeightVelocity = FVector::DotProduct(Root->GetPhysicsLinearVelocity(),UpVector) * UpVector;

		ThrustForce = (-GravityForceVector * UKismetMathLibrary::Exp(ScaleHeight) - HoverDampingFactor * HeightVelocity * UKismetMathLibrary::Exp(ScaleHeight)).GetClampedToMaxSize(80000.f);
	}

	//Root->AddForce(ThrustForce + GravityForceVector, FName(), true);
	Root->AddForce(ShipWeight * (ThrustForce + GravityForceVector), FName(), false); // From acceleration to force -> F = ma
}
*/

void APlayerShipPhysics::AddForce(FVector_NetQuantize End, int Num) const
{
	FVector Start, CompLocation, UpVector = GetActorUpVector();

	switch (Num)
	{
	case 1:
		Start = Thrust1->GetComponentLocation();
		CompLocation = Thrust1->GetComponentLocation();
		break;
	case 2:
		Start = Thrust2->GetComponentLocation();
		CompLocation = Thrust2->GetComponentLocation();
		break;
	case 3:
		Start = Thrust3->GetComponentLocation();
		CompLocation = Thrust3->GetComponentLocation();
		break;
	case 4:
		Start = Thrust4->GetComponentLocation();
		CompLocation = Thrust4->GetComponentLocation();
		break;
	default:
		Start = FVector::ZeroVector;
		break;
	}
	
	// Get distance from thrust point to ground
	const float NormalizedDistance = UKismetMathLibrary::Vector_Distance(Start, End) / TargetHeight;
	
	const float ZVelocity = FVector::DotProduct(Root->GetPhysicsLinearVelocity(), GetActorUpVector());

	const float Constant = Gravity/5.f;

	//UE_LOG(LogTemp, Warning, TEXT("NormDist: %f"), NormalizedDistance)

	// If we are close enough to the ground to give thrust
	if (NormalizedDistance < 1.f)
	{
		const FVector ThrustForce = (Constant * HoverForceCurve->GetFloatValue(NormalizedDistance) * UpVector);
		const FVector Damping = ZVelocity * -UpVector * (CustomCurve2->GetFloatValue(NormalizedDistance) + 1) * 2000;
		Root->AddForceAtLocation(ThrustForce, CompLocation);
		Root->AddForceAtLocation(Damping, CompLocation);
	}
	
}


void APlayerShipPhysics::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }

	if (OtherActor->IsA(ACheckPoint::StaticClass()))
	{
		ACheckPoint* CheckPoint_Temp = Cast<ACheckPoint>(OtherActor);

		if (CheckPoint_Temp)
		{
			UE_LOG(LogTemp, Warning, TEXT("CheckPoint_Temp: %d | PlayerNumber: %d"), CheckPoint_Temp->ThisCheckpointNumber, GameInstance->PlayerCheckpointNumber);

			if (GameInstance->TimeAttackMode)
			{
				if (GameInstance->PlayerCheckpointNumber - 1 == CheckPoint_Temp->ThisCheckpointNumber || (CheckPoint_Temp->bIsFirst_CheckPoint && GameInstance->PlayerCheckpointNumber <= 2))
				{
					CheckPoint_Last = CheckPoint_Temp;
					GameInstance->NewCheckPoint = true;
					GameInstance->CheckPoint_Connected = true;

					if (CheckPoint_Temp->bIsFirst_CheckPoint)
					{
						GameInstance->PlayerCheckpointNumber = CheckPointsAmount + 2;
					}
					if (CheckPoint_Temp->bIsGoal)
					{
						GameInstance->PlayerCheckpointNumber = CheckPointsAmount;

						GameInstance->CurrentLap_Counter += 1;

						if (GameInstance->CurrentLap_Counter <= 3)
						{
							UGameplayStatics::PlaySound2D(GetWorld(), NewLap_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
						}
						else
						{
							UGameplayStatics::PlaySound2D(GetWorld(), RaceWon_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
						}
					}
					else
					{
						GameInstance->PlayerCheckpointNumber -= 1;
					}

					if (GameInstance->TimeAttackMode == true)
					{
						GameInstance->TimeCount += GameInstance->TimeAdded;
					}

					UE_LOG(LogTemp, Warning, TEXT("Checkpoint - Time Attack - SUCESS, No.%d"), CheckPoint_Temp->ThisCheckpointNumber);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Checkpoint - Time Attack - Not in Range, No.%d"), CheckPoint_Temp->ThisCheckpointNumber);
				}
			}
			else
			{
				if (GameInstance->PlayerCheckpointNumber == CheckPoint_Temp->ThisCheckpointNumber - 1)
				{
					CheckPoint_Last = CheckPoint_Temp;
					GameInstance->NewCheckPoint = true;
					GameInstance->CheckPoint_Connected = true;

					if (CheckPoint_Temp->bIsGoal)
					{
						GameInstance->PlayerCheckpointNumber = 0;

						GameInstance->CurrentLap_Counter += 1;

						if (GameInstance->CurrentLap_Counter <= 3)
						{
							UGameplayStatics::PlaySound2D(GetWorld(), NewLap_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
						}
						else
						{
							UGameplayStatics::PlaySound2D(GetWorld(), RaceWon_Sound, 1.f * GameInstance->GlobalVolumeMultiplier);
						}
					}
					else
					{
						GameInstance->PlayerCheckpointNumber += 1;
					}

					if (GameInstance->TimeAttackMode == true)
					{
						GameInstance->TimeCount += GameInstance->TimeAdded;
					}

					UE_LOG(LogTemp, Warning, TEXT("Checkpoint - Adventure Mode - SUCESS, No.%d"), CheckPoint_Temp->ThisCheckpointNumber);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Checkpoint - Adventure Mode - Not in Range, No.%d"), CheckPoint_Temp->ThisCheckpointNumber);
				}
			}
		}
	}
}


void APlayerShipPhysics::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) { return; }
	UE_LOG(LogTemp, Warning, TEXT("We hit something."))
	if (HitSound1 && HitSound2 && HitSoundCooldown > 0.5f)
	{
		USoundBase* Sound;
		switch (FMath::RandRange(0,1))
		{
		case 0:
			Sound = HitSound1;
			break;
		case 1:
			Sound = HitSound2;
			break;
		default:
			Sound = HitSound1;
			break;
		}
		
		if (GameInstance)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Hit.ImpactPoint, 0.6f * GameInstance->GlobalVolumeMultiplier);
		}
		
		HitSoundCooldown = 0.f;
	}

	if (GameInstance)
	{
		// The amount of damage to cause to the player when they crash at x speed
		const float Amount = FMath::Lerp(5, 30, Speed/18000.f);
		GameInstance->AddHealth(-Amount);
	}
}
