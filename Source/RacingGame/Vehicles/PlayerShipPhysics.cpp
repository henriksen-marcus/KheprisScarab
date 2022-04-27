// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipPhysics.h"
#include "../Global_Variables.h"
#include "../Environment/CheckPoint.h"
#include "../Other/Bullet.h"
#include "../RacingGameGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
APlayerShipPhysics::APlayerShipPhysics()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<UBoxComponent>(TEXT("RootReplacement"));
	Root->InitBoxExtent(FVector(200.f, 200.f, 50.f));
	SetRootComponent(Root);
	Root->SetSimulatePhysics(true);
	Root->SetEnableGravity(false);
	Root->SetLinearDamping(3.f);
	Root->SetAngularDamping(20.f);
	Root->SetPhysicsMaxAngularVelocityInDegrees(250.f);
	TEnumAsByte<ECanBeCharacterBase> temp;
	temp = ECanBeCharacterBase::ECB_No;
	Root->CanCharacterStepUpOn = temp;
	Root->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Root->SetCollisionProfileName(FName("Ship")); // Collision mot working properly? Make sure there is a collision preset named "Ship".

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	BaseMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/3DAssets/Characters/Spaceship/spaceship.spaceship'")).Object);
	BaseMesh->SetRelativeScale3D(FVector(.3f, .3f, .3f));
	BaseMesh->SetupAttachment(GetRootComponent());

	// SpringArm
	{
		BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BackSpringArm"));
		BackSpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
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

		FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FrontSpringArm"));
		FrontSpringArm->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
		FrontSpringArm->TargetArmLength = 1500.f;
		FrontSpringArm->bEnableCameraLag = true;
		//FrontSpringArm->CameraLagSpeed = 100.f; // Lower = More delay
		FrontSpringArm->bEnableCameraRotationLag = false;
		//FrontSpringArm->CameraRotationLagSpeed = 30.f;
		FrontSpringArm->bDoCollisionTest = true;
		FrontSpringArm->SetupAttachment(GetRootComponent());

		ActiveSpringArm = BackSpringArm;
	}

	// Camera
	{
		BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("BackCamera"));
		BackCamera->bUsePawnControlRotation = false;
		BackCamera->SetupAttachment(BackSpringArm, USpringArmComponent::SocketName);
		BackCamera->SetRelativeRotation(FRotator(12.f, 0.f, 0.f));

		FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FrontCamera"));
		FrontCamera->bUsePawnControlRotation = false;
		FrontCamera->SetupAttachment(FrontSpringArm, USpringArmComponent::SocketName);
		FrontCamera->SetRelativeRotation(FRotator(5.f, 0.f, 0.f));
		FrontCamera->SetActive(false);
	}

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComp->SetSound(ConstructorHelpers::FObjectFinder<USoundCue>(TEXT("SoundCue'/Game/SoundEffects/EngineSounds/Running/EngineRunning2.EngineRunning2'")).Object);
	AudioComp->SetupAttachment(GetRootComponent());

	StartSound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/EngineSounds/starting-car.starting-car'")).Object;
	BoostSound = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/SoundEffects/Bang/firework-boom.firework-boom'")).Object;

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

		Thrust1->SetRelativeLocationAndRotation(FVector(200.f, -200.f, 0.f), FRotator(-60.f, -45.f, 0.f));
		Thrust2->SetRelativeLocationAndRotation(FVector(200.f, 200.f, 0.f), FRotator(-60.f, 45.f, 0.f));
		Thrust3->SetRelativeLocationAndRotation(FVector(-200.f, -200.f, 0.f), FRotator(-120.f, 45.f, 0.f));
		Thrust4->SetRelativeLocationAndRotation(FVector(-200.f, 200.f, 0.f), FRotator(-120.f, -45.f, 0.f));

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

}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	Root->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);
	Root->OnComponentHit.AddDynamic(this, &APlayerShipPhysics::OnHitBegin);
	InitialLocation = GetActorLocation();
	InitialTargetHeight = TargetHeight;
	InitialBackSpringArmRotation = BackSpringArm->GetRelativeRotation();
	InitialFrontSpringArmRotation = FrontSpringArm->GetRelativeRotation();
	AudioComp->Deactivate();


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
}


void APlayerShipPhysics::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Local + Global gravity, added a little more force the the opposing (local) force
	const FVector CombinedGravity = ( FVector::DownVector - (GetActorUpVector() * 1.08f) ).GetSafeNormal();
	Root->AddForce(CombinedGravity * (Gravity));

	// Local forwards force
	Root->AddForce(GetActorForwardVector() * Force.X, FName(), true);

	MovementUpdate();
	//RaycastHover();

	// Limit speed
	if (Root->GetPhysicsLinearVelocity().Size() > 30000.f && !bIsDashing)
	{
		Root->SetPhysicsLinearVelocity(Root->GetPhysicsLinearVelocity().GetSafeNormal() * 20000.f);
	}

	// Drag
	if (bEnableDrag)
	{
		FVector LinearVelocity = Root->GetPhysicsLinearVelocity();
		LinearVelocity.Z = 0.f;
		//Root->AddForce(-LinearVelocity * 0.8f, FName(), true);
	}


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

	CameraCenteringTimer += DeltaTime;
	ShootTimer += DeltaTime;
	JumpTimer += DeltaTime;

	FHitResult HitRes;
	FCollisionQueryParams Params;

	if (GetWorld()->LineTraceSingleByChannel(HitRes, GetActorLocation(), GetActorLocation() + FVector::DownVector * 1000, ECC_Visibility, Params))
	{
		if (HitRes.PhysMaterial == MyMat->PhysMaterial)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found sand physical material."))
		}
	}
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

	PlayerInputComponent->BindAction("ScrollUp", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::CameraZoomIn);
	PlayerInputComponent->BindAction("ScrollDown", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::CameraZoomOut);

	PlayerInputComponent->BindAction("CameraSwap", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::CameraSwap);
	PlayerInputComponent->BindAction("CameraSwap", EInputEvent::IE_Released, this, &APlayerShipPhysics::CameraSwap);

	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Reload);

	PlayerInputComponent->BindAction("Respawn", EInputEvent::IE_Pressed, this, &APlayerShipPhysics::Respawn);
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //


void APlayerShipPhysics::Forward(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{// Determine if there is input
			bForwardHasInput = !(Value == 0);

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	if (Value)
	{
		Force.X = FMath::FInterpTo(Force.X, (Value * 12.f * SpeedBoost * SpeedMultiplier * ForwardsSpeed * LinearDampingReduction), GetWorld()->GetDeltaSeconds(), 1.5f);
	}
	else
	{
		FVector LinearVel = Root->GetPhysicsLinearVelocity();
		LinearVel.Z *= 0.2f;
		Root->AddForce(LinearVel * 0.98f * LinearDampingReduction, FName(), true);
		Force.X = 0.f;
	}
}

void APlayerShipPhysics::Turn(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{// Determine if there is input
			bRollHasInput = !(Value == 0);

			// Roll
			float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;

			// Interpolate rotation towards target
			NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

			// Yaw
			YawMove = FMath::FInterpTo(YawMove, Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);
		}
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
	if (GameInstance)
	{
		// Reset rotation
		if (ActiveSpringArm == BackSpringArm)
		{
			ActiveSpringArm->SetRelativeRotation(FMath::RInterpTo(ActiveSpringArm->GetRelativeRotation(), InitialBackSpringArmRotation, GetWorld()->GetDeltaSeconds(), 6.f));
		}
		else
		{
			ActiveSpringArm->SetRelativeRotation(FMath::RInterpTo(ActiveSpringArm->GetRelativeRotation(), InitialFrontSpringArmRotation, GetWorld()->GetDeltaSeconds(), 6.f));
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
	}
}

void APlayerShipPhysics::Shoot(const float Value)
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{// Was 0.085f
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
			/*else if (GunClickSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), GunClickSound, 0.8f);
			}*/
		}
	}
}

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

void APlayerShipPhysics::Dash()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			if (GameInstance->BoostPickup)
			{
				GameInstance->BoostPickup = false;

				if (bIsDashing)
				{
					return;
				}

				static float CamFovChange = 15.f;
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
}

void APlayerShipPhysics::Jump()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
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
}

void APlayerShipPhysics::Respawn()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			ARacingGameGameModeBase* GamemodeBase = Cast<ARacingGameGameModeBase>(GetWorld()->GetAuthGameMode());
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
}

void APlayerShipPhysics::Crouch()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			TargetHeight /= 2.f;
		}
	}
}

void APlayerShipPhysics::CrouchEnd()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			TargetHeight *= 2.f;
		}
	}
}

void APlayerShipPhysics::CameraZoomIn()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			if (bIsDashing) { return; }
			TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength - 200.f, 200.f, 5000.f);
		}
	}
}

void APlayerShipPhysics::CameraZoomOut()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			if (bIsDashing) { return; }
			TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength + 200.f, 200.f, 5000.f);
		}
	}
}

void APlayerShipPhysics::SloMo(float Amount, float Duration)
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			FTimerHandle Handle;
			FTimerDelegate TimerDelegate1, TimerDelegate2;

			// This lambda function will loop to ensure the interpolation works
			TimerDelegate1.BindLambda([&]
				{
					float CurrentDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
					float NewDilation = FMath::FInterpTo(CurrentDilation, Amount, GetWorld()->GetDeltaSeconds(), 5.f);
					UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);
				});

			GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, TimerDelegate1, GetWorld()->GetDeltaSeconds(), true);

			// This lambda function will stop the loop after the given duration
			TimerDelegate2.BindLambda([&]
				{
					GetWorld()->GetTimerManager().ClearTimer(TimeDilationHandle);
					UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
				});

			GetWorld()->GetTimerManager().SetTimer(Handle, TimerDelegate2, 1.f, false);
		}
	}
}

void APlayerShipPhysics::CameraSwap()
{
	if (GameInstance)
	{
		if (GameInstance->RaceStartOFF == false)
		{
			if (BackCamera->IsActive())
			{
				BackCamera->SetActive(false);
				FrontCamera->SetActive(true);
				ActiveSpringArm = FrontSpringArm;
				BackSpringArm->SetRelativeRotation(InitialBackSpringArmRotation);
			}
			else
			{
				FrontCamera->SetActive(false);
				BackCamera->SetActive(true);
				ActiveSpringArm = BackSpringArm;
				FrontSpringArm->SetRelativeRotation(InitialFrontSpringArmRotation);
			}
		}
	}
}

void APlayerShipPhysics::MovementUpdate()
{
	float RayCastLength = InitialTargetHeight * 2 + 500;
	TArray<FHitResult> HitPoints;
	HitPoints.Init(FHitResult(), 4);

	// Used to determine if we are in the air - Counter == 4 -> Four raycast misses
	int32 Counter{};

	// Start raycast
	for (int i{}; i < 4; i++)
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

	// Decide what to do based on how many raycasts were hits
	for (int i{}; i < HitPoints.Num(); i++)
	{
		bool bBlockingHit = false;
		if (HitPoints[i].bBlockingHit) { bBlockingHit = true; }
			switch (i)
			{
			case 0:
				AddForce(HitPoints[0].Location, 1, bBlockingHit);
				break;
			case 1:
				AddForce(HitPoints[1].Location, 2, bBlockingHit);
				break;
			case 2:
				AddForce(HitPoints[2].Location, 3, bBlockingHit);
				break;
			case 3:
				AddForce(HitPoints[3].Location, 4, bBlockingHit);
				break;
			default:
				break;
			}
	}

	if (Counter == 4)
	{
		// Ship is in the air
		FRotator CurrentRotation = GetActorRotation();
		CurrentRotation.Pitch = FMath::FInterpTo(CurrentRotation.Pitch, -5.f, GetWorld()->GetDeltaSeconds(), 0.6f);
		CurrentRotation.Roll = FMath::FInterpTo(CurrentRotation.Roll, 0.f, GetWorld()->GetDeltaSeconds(), 1.f);
		SetActorRotation(CurrentRotation);
		Root->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		Root->SetLinearDamping(0.4f);
		LinearDampingReduction = 0.1f;
		bEnableDrag = true;
	}
	else
	{
		Root->SetLinearDamping(InitialLinearDamping);
		LinearDampingReduction = 1.f;
		bEnableDrag = false;
	}
}

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

void APlayerShipPhysics::AddForce(FVector_NetQuantize End, int Num, bool bHit) const
{
	if (!bHit)
	{
		return;
	}

	FVector ThrustForce, Start, CompLocation, UpVector = GetActorUpVector();
	float Constant = Gravity/4.f;//50000.f;
	float Distance;

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
	Distance = UKismetMathLibrary::Vector_Distance(Start, End) / TargetHeight;

	// Determine thrust force based on distance
	ThrustForce = Constant * HoverForceCurve->GetFloatValue(Distance) * UpVector;

	// Add local downwards force if the ship is higher than it should be (TargetHeight)
	if (Distance > 1.f) { Root->AddForce(-UpVector * MinusHoverForceCurve->GetFloatValue(Distance)); }

	// Add the force at the specified location
	Root->AddForceAtLocation(ThrustForce, CompLocation);
}


/*void APlayerShipPhysics::AddForce(FVector_NetQuantize End, int Num, bool bHit) const
{
	FVector Start, CompLocation;

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

	FVector UpVector = GetActorUpVector();
	FVector GravityForceVector = -GetActorUpVector() * 2000;
	static float HoverForceReduction = 40; // S
	static float HoverDampingFactor = 6.f; // K
	float MaxClamp = 80000.f;

	// Get distance from thrust point to ground - Equation credit: Anders �sb�
	const float Distance = UKismetMathLibrary::Vector_Distance(Start, End);
	const float ScaleHeight = (TargetHeight - Distance) / HoverForceReduction;
	const FVector HeightVelocity = FVector::DotProduct(Root->GetPhysicsLinearVelocity(),UpVector) * UpVector;
	FVector ThrustForce = FVector::ZeroVector;

	if (bHit){
		ThrustForce = (-GravityForceVector * UKismetMathLibrary::Exp(ScaleHeight) - HoverDampingFactor * HeightVelocity /** UKismetMathLibrary::Exp(ScaleHeight)#1#).GetClampedToMaxSize(MaxClamp);
	}

	Root->AddForceAtLocation(ShipWeight * ThrustForce / 4, CompLocation);
	Root->AddForceAtLocation(ShipWeight * GravityForceVector / 4, CompLocation);

	// Gravity line
	DrawDebugLine(GetWorld(), Start, Start + (GravityForceVector) / 10, FColor::Blue, false, -1, 0, 14.f);

	// Force line
	DrawDebugLine(GetWorld(), Start, Start + (ThrustForce) / 10, FColor::Red, false, -1, 0, 14.f);

	// Local up vector
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector() * 600, FColor::Green, false, -1, 0, 12.f);

	// Absolute down vector
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + FVector::DownVector * 600, FColor::Black, false, -1, 0, 12.f);#1#
}*/

/*if (ThrustForce.GetSafeNormal() == -GetActorUpVector().GetSafeNormal())
	{
		ThrustForce = -ThrustForce;
	}*/


void APlayerShipPhysics::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Ship Overlapped with something."))
	if (!OtherActor || OtherActor == this || !OtherComponent) { return; }
}


void APlayerShipPhysics::OnHitBegin(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this) { return; }

}
