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

// Sets default values
APlayerShipPhysics::APlayerShipPhysics()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Root
	{
		Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
		Root->InitBoxExtent(FVector(200.f, 200.f, 50.f));
		SetRootComponent(Root);
		Root->SetSimulatePhysics(true);
		Root->SetEnableGravity(false);
		Root->SetLinearDamping(0.f);
		Root->SetAngularDamping(20.f);
		Root->SetPhysicsMaxAngularVelocityInDegrees(200.f);
		TEnumAsByte<ECanBeCharacterBase> temp;
		temp = ECanBeCharacterBase::ECB_No;
		Root->CanCharacterStepUpOn = temp;
		Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Root->SetCollisionProfileName(FName("Ship")); // Collision mot working properly? Make sure there is a collision preset named "Ship".
	}
	
	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetSkeletalMesh(ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/3DAssets/New_Ship/Ship_Rigged.Ship_Rigged'")).Object);
	BaseMesh->SetRelativeScale3D(FVector(.3f, .3f, .3f));
	BaseMesh->SetupAttachment(GetRootComponent());

	// SpringArm
	{
		BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BackSpringArm"));
		BackSpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f));
		//SpringArm->SetUsingAbsoluteRotation(false);
		//SpringArm->bUsePawnControlRotation = true;
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
		BehindSpringArm->bEnableCameraLag = true;
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
		FrontCamera->SetupAttachment(GetRootComponent());
		FrontCamera->SetRelativeLocation(FVector(160.f, 0.f, -20.f));
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

	//MovementComponent = CreateDefaultSubobject<UHoveringMovementComponent>(TEXT("CustomMovementComponent"));
	//BulletClassToSpawn = ABullet::StaticClass();
	Root->OnComponentHit.AddDynamic(this, &APlayerShipPhysics::OnHit);
}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	Root->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);
	
	InitialLocation = GetActorLocation();
	InitialTargetHeight = TargetHeight;
	InitialBackSpringArmRotation = BackSpringArm->GetRelativeRotation();
	InitialBehindSpringArmRotation = BehindSpringArm->GetRelativeRotation();
	AudioComp->Deactivate();

	BehindCamera->SetActive(false);
	FrontCamera->SetActive(false);

	//UE_LOG(LogTemp, Warning, TEXT("Date: %s"), *FDateTime::Now().ToString())

	if (StartSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), StartSound, 0.2f);

		FTimerHandle TH_BeginPlay;
		FTimerDelegate TD_BeginPlay;
		// Lambda expression
		TD_BeginPlay.BindLambda([&]{ AudioComp->FadeIn(0.8f); AudioComp->PitchMultiplier = 1.f; });
		GetWorld()->GetTimerManager().SetTimer(TH_BeginPlay, TD_BeginPlay, 0.6f, false);
	}
	else
	{
		AudioComp->Activate();
	}

	ShipWeight = Root->GetMass();
	Gravity = ShipWeight * 9.81f * 100 * 4 * GravityScalar;
	UE_LOG(LogTemp, Warning, TEXT("Gravity: %f"), Gravity)
	InitialLinearDamping = Root->GetLinearDamping();

	GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
	GamemodeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());
}


void APlayerShipPhysics::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Local + Global gravity, added a little more force the the opposing (local) force
	const FVector CombinedGravity = ( FVector::DownVector - (GetActorUpVector() * 1.1f) ).GetSafeNormal();
	Root->AddForce(CombinedGravity * (Gravity));

	// Local forwards force
	Root->AddForce(GetActorForwardVector() * Force.X, FName(), true);

	MovementUpdate();
	//RaycastHover();

	// Limit speed
	if (Root->GetPhysicsLinearVelocity().Size() > 15000.f * SpeedMultiplier && !bIsDashing)
	{
		Root->SetPhysicsLinearVelocity(Root->GetPhysicsLinearVelocity().GetSafeNormal() * 20000.f);
	}

	// Drag
	if (bEnableDrag)
	{
		FVector LinearVelocity = Root->GetPhysicsLinearVelocity();
		LinearVelocity.Z = 0.f;
		Root->AddForce(-LinearVelocity * 0.85f, FName(), true);
	}

	Root->SetPhysicsLinearVelocity(FMath::VInterpTo(Root->GetPhysicsLinearVelocity(), GetActorForwardVector() * Root->GetPhysicsLinearVelocity().Size(), DeltaTime, 0.9f));

	if (bIsJumping)
	{
		Root->AddForce(GetActorUpVector() * Gravity * 10);
	}

	// Engine dynamic audio
	const float Speed = Root->GetPhysicsLinearVelocity().Size();

	if (bLogSpeed)
	{
		UE_LOG(LogTemp,Warning,TEXT("Speed is: %f"), Speed);
	}

	if (AudioComp && CustomCurve2)
	{
		// Interp to smooth out speed/audio fluctuations
		AudioComp->PitchMultiplier = FMath::FInterpTo(AudioComp->PitchMultiplier, (CustomCurve2->GetFloatValue(Speed/20000.f) + 1), DeltaTime, 1.5f);
		AudioComp->SetPitchMultiplier(AudioComp->PitchMultiplier);
	}

	AddActorLocalRotation(FRotator(0.f, YawMove, 0.f));

	// Cosmetic mesh rotation
	BaseMesh->SetRelativeRotation(FRotator(NextPitchPosition, 0.f, NextRollPosition));

	CameraUpdate();

	FVector HitLoc;
	if (CheckSurface(HitLoc) == FString("PM_Sand"))
	{
		SpawnSandEffect(HitLoc);
	}
	else if (SandSystemPtr)
	{
		SandSystemPtr->DestroyInstance();
		SandSystemPtr = nullptr;
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

	PlayerInputComponent->BindAxis("Shoot", this, &APlayerShipPhysics::Shoot);

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
		bForwardHasInput = !(Value == 0);

		// If there is input, set rotation target to based on input value, else set target to 0
		const float TargetPitch = bForwardHasInput ? Value > 0.f ? -5.0f : 5.f : 0.f;

		// Interpolate rotation towards target
		if (BaseMesh)
		{
			NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 5.0f);
		}

		if (Value)
		{
			//Force.X = (Value * 2.f * SpeedBoost * SpeedMultiplier * ForwardsSpeed);
			//Force.X = Value * 6.f * SpeedBoost * SpeedMultiplier * ForwardsSpeed;
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
		bRollHasInput = !(Value == 0);

		// Roll
		float TargetRoll = bRollHasInput ? Value > 0 ? 5.f : -5.f : 0.f;

		// Interpolate rotation towards target
		if (BaseMesh)
		{
			NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Shiiiieeeet"))
		}
		

		// Yaw
		YawMove = FMath::FInterpTo(YawMove, Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 5.f);
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
}

FString APlayerShipPhysics::CheckSurface(FVector &HitLocation)
{
	/** How far to check for underlying surface */
	static float CheckDistance = 1000.f;
	
	FHitResult HitRes;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + FVector::DownVector * CheckDistance;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	
	if (GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_Visibility))
	{
		if (HitRes.bBlockingHit)
		{
			if (IsValid(HitRes.Component.Get()))
			{
				if (IsValid(HitRes.Component.Get()->GetMaterial(0)))
				{
					if (IsValid(HitRes.Component.Get()->GetMaterial(0)->GetPhysicalMaterial()))
					{
						UPhysicalMaterial* PhysMat = HitRes.Component.Get()->GetMaterial(0)->GetPhysicalMaterial();
						if (PhysMat)
						{
							HitLocation = HitRes.Location;
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
			FVector RelativeLocation = FVector(0.f, 0.f, HitLoc.Z - GetActorLocation().Z);
			SandSystemPtr = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NS_SandSystem, HitLoc, FRotator::ZeroRotator);
		}
	}
	else
	{
		SandSystemPtr->SetWorldLocation(HitLoc);
	}
}

void APlayerShipPhysics::Shoot(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		// Was 0.085f
		if (!Value || ShootTimer < 0.1f) { return; }

		ShootTimer = 0.f;
		if (GameInstance)
		{
			if (GameInstance->CurrentAmmo > 0) {
				--GameInstance->CurrentAmmo;
				if (GetWorld())
				{
					// Spawn bullet
					ABullet* BulletRef = GetWorld()->SpawnActor<ABullet>(BulletClassToSpawn, BulletSpawnPoint->GetComponentLocation(), GetActorRotation());
					if (BulletRef)
					{
						BulletRef->SetBulletOwner(this);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("BulletRef not valid!"))
					}
					//Root->AddImpulse(-GetActorForwardVector() * 100000.f);
				}
			}
		}
		else if (GunClickSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), GunClickSound, 0.8f);
		}
	}
}

/*
void APlayerShipPhysics::Reload()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			if (bIsReloading) { return; }

			bIsReloading = true;

			if (ReloadSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), ReloadSound);
			}

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			// Lambda expression
			TimerDelegate.BindLambda([&]
				{
					UGlobal_Variables* GameInstance = Cast<UGlobal_Variables>(GetGameInstance());
					if (GameInstance)
					{
						GameInstance->CurrentAmmo = GameInstance->MaxAmmo;
						bIsReloading = false;
					}
				});

			if (GameInstance)
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, GameInstance->DashTimer, false);
			}
		}
	}
}
*/

void APlayerShipPhysics::Dash()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		if (GameInstance->BoostPickup)
			{
				GameInstance->BoostPickup = false;

				if (bIsDashing)
				{
					return;
				}

				static float CamFovChange = 25.f;
				static float SpringArmChange = 400.f;

				TargetCameraFOV += CamFovChange;
				TargetSpringArmLength -= SpringArmChange;
				BackSpringArm->CameraLagSpeed = 35.f;
				BackSpringArm->CameraRotationLagSpeed = 20.f;
				SpeedBoost = MaxSpeedBoost;
				bIsDashing = true;

				if (BoostSound)
				{
					UGameplayStatics::PlaySound2D(GetWorld(), BoostSound, 0.5f);
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

				if (GameInstance)
				{
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, GameInstance->DashTimer, false);
				}
			}
	}
}

void APlayerShipPhysics::Jump()
{
	if (GameInstance)
	{
		if (GameInstance->bRaceNotStarted) { return; }
		
		if (bIsJumping || JumpTimer < 2.f)
		{
			return;
		}

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
		
		if (GamemodeBase)
		{
			if (GameInstance->PlayerCheckpointNumber == 1)
			{
				SetActorLocation(GamemodeBase->Checkpoints[0]->SpawnPointArrow->GetRelativeLocation());

				UE_LOG(LogTemp, Warning, TEXT("Respawn 1 - Success"));
			}

			if (GameInstance->PlayerCheckpointNumber == 2)
			{
				SetActorLocation(GamemodeBase->Checkpoints[1]->SpawnPointArrow->GetRelativeLocation());

				UE_LOG(LogTemp, Warning, TEXT("Respawn 2 - Success"));
			}

			if (GameInstance->PlayerCheckpointNumber == 3)
			{
				SetActorLocation(GamemodeBase->Checkpoints[2]->SpawnPointArrow->GetRelativeLocation());

				UE_LOG(LogTemp, Warning, TEXT("Respawn 3 - Success"));
			}
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

void APlayerShipPhysics::SloMo(float Amount, float Duration)
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
}

void APlayerShipPhysics::LookBehind()
{
	UE_LOG(LogTemp, Warning, TEXT("LookBehind"))
	// Swap active camera based on which is active.
	if (BehindCamera->IsActive())
	{
		UE_LOG(LogTemp, Warning, TEXT("BehindCamera Active!"))
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
	float RayCastLength = InitialTargetHeight * 2;
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
			UE_LOG(LogTemp, Warning, TEXT("Error in APlayerShipPhysics::MovementUpdate()"))
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
	// End raycast

	FHitResult HitRes;
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() - GetActorUpVector() * (RayCastLength + 1000);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Red);
	// Raycast for ground sticking
	if (GetWorld()->LineTraceSingleByChannel(HitRes, Start, End, ECC_Visibility))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit ground."))
		float NormDist = UKismetMathLibrary::Vector_Distance(Start, HitRes.Location) / TargetHeight;

		if (NormDist > 1.f)
		{
			Root->AddForce(-GetActorUpVector() * Gravity*2 * MinusHoverForceCurve->GetFloatValue(NormDist));
			FVector Damping = FVector::DotProduct(Root->GetPhysicsLinearVelocity(), -GetActorUpVector()) * -GetActorUpVector() * 1500;
			Root->AddForce(Damping);
			DrawDebugLine(GetWorld(), Start, End, FColor::Green);
		}
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("not hit"))
	}

	// Decide what to do based on how many raycasts were hits
	for (int i{}; i < HitPoints.Num(); i++)
	{
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
		// Ship is in the air
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, -5.f, GetWorld()->GetDeltaSeconds(), 0.6f);
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
	FVector ThrustForce, Start, CompLocation, UpVector = GetActorUpVector();
	float Constant = Gravity/4.5f;
	float NormalizedDistance;

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
	NormalizedDistance = UKismetMathLibrary::Vector_Distance(Start, End) / TargetHeight;

	// Determine thrust force based on distance
	ThrustForce = Constant * HoverForceCurve->GetFloatValue(NormalizedDistance) * UpVector;

	// Add local downwards force if the ship is higher than it should be (TargetHeight)
	if (NormalizedDistance > 1.f)
	{
		//Root->AddForce(-UpVector * MinusHoverForceCurve->GetFloatValue(Distance));
		//Root->AddForceAtLocation(-UpVector * (HoverForceCurve->GetFloatValue(1 - (NormalizedDistance-1)) - 1) * Constant/2, CompLocation);
		//Root->AddForceAtLocation(-UpVector * MinusHoverForceCurve->GetFloatValue(NormalizedDistance) * Constant, CompLocation);
	}
	else
	{
		// Add the force at the specified location
		Root->AddForceAtLocation(ThrustForce, CompLocation);
		// Damping
		FVector Damping = FVector::DotProduct(Root->GetPhysicsLinearVelocity(), GetActorUpVector()) * -(ThrustForce.GetSafeNormal()) * (CustomCurve2->GetFloatValue(NormalizedDistance) + 1) * 2500;
		Root->AddForceAtLocation(Damping, CompLocation);
	}
}


void APlayerShipPhysics::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Ship Overlapped with something."))
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }
}


void APlayerShipPhysics::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) { return; }
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
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, Hit.ImpactPoint, 0.6f);
		HitSoundCooldown = 0.f;
	}
}
