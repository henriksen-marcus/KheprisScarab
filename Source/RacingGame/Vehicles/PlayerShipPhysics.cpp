// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShipPhysics.h"

#include <ThirdParty/openexr/Deploy/OpenEXR-2.3.0/OpenEXR/include/ImathEuler.h>

#include "../Global_Variables.h"
#include "../Other/Bullet.h"
#include "Kismet/KismetMathLibrary.h"

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
		BackSpringArm->bEnableCameraLag = true;
		BackSpringArm->CameraLagSpeed = 30.f; // Lower = More delay
		BackSpringArm->bEnableCameraRotationLag = true;
		BackSpringArm->CameraLagMaxDistance = 2000.f;
		BackSpringArm->CameraRotationLagSpeed = 15.f;
		BackSpringArm->bDoCollisionTest = true;
		BackSpringArm->SetupAttachment(GetRootComponent());

		FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FrontSpringArm"));
		FrontSpringArm->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
		FrontSpringArm->TargetArmLength = 1000.f;
		FrontSpringArm->bEnableCameraLag = true;
		FrontSpringArm->CameraLagSpeed = 40.f; // Lower = More delay
		FrontSpringArm->bEnableCameraRotationLag = true;
		FrontSpringArm->CameraRotationLagSpeed = 20.f;
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

	/** Adrian */
	//Variables
	//MaxHealth = 5.f;
	//CurrentHealth = MaxHealth;
	//MaxAmmo = 10; // Is now 50 - Marcus
	//CurrentAmmo = MaxAmmo;
	//TimeCount = 60;
	//TimeAdded = 15;
}


void APlayerShipPhysics::BeginPlay()
{
	Super::BeginPlay();
	Root->OnComponentBeginOverlap.AddDynamic(this, &APlayerShipPhysics::OnOverlapBegin);
	InitialLocation = GetActorLocation();
	InitialTargetHeight = TargetHeight;
	InitialBackSpringArmRotation = BackSpringArm->GetRelativeRotation();
	InitialFrontSpringArmRotation = FrontSpringArm->GetRelativeRotation();
	AudioComp->Deactivate();
	
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
	if (Root->GetPhysicsLinearVelocity().Size() > 20000.f && !bIsDashing)
	{
		Root->SetPhysicsLinearVelocity(Root->GetPhysicsLinearVelocity().GetSafeNormal() * 20000.f);
	}

	// Drag
	/*FVector LinearVelocity = Root->GetPhysicsLinearVelocity();
	LinearVelocity.Z = 0.f;
	Root->AddForce(-LinearVelocity * ShipWeight * 0.5f);*/

	UE_LOG(LogTemp, Warning, TEXT("LinVel: %s"), *Root->GetPhysicsLinearVelocity().ToString())

	if (bIsJumping)
	{
		Root->AddForce(GetActorUpVector() * Gravity * 10);
	}
	
	// Engine dynamic audio
	const float Speed = Root->GetComponentVelocity().Size();

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
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //


void APlayerShipPhysics::Forward(const float Value)
{
	// Determine if there is input
	bForwardHasInput = !(Value == 0);
	
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	const float TargetPitch = bForwardHasInput ? Value > 0.f ? -10.0f : 10.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);

	Force.X = Value * 8.f * SpeedBoost * SpeedMultiplier * ForwardsSpeed;
	/*Force.X = Value ?
		FMath::FInterpTo(Force.X, (Value * 10.f * ShipWeight * SpeedBoost * SpeedMultiplier * ForwardsSpeed), GetWorld()->GetDeltaSeconds(), 2.5f) :
		FMath::FInterpTo(Force.X, 0.f, GetWorld()->GetDeltaSeconds(), 1.5f);*/
	
	//ActualSpeed = Value > 0.f ? ForwardsSpeed : FMath::FInterpTo(ActualSpeed, 0.f, GetWorld()->GetDeltaSeconds(), 0.5f);
	//UE_LOG(LogTemp, Warning, TEXT("AcutalSpeedX: %f"), ActualSpeed)
}

void APlayerShipPhysics::Turn(const float Value)
{
	// Determine if there is input
	bRollHasInput = !(Value == 0);

	// Roll
	float TargetRoll = bRollHasInput ? Value > 0 ? 15.0f : -15.0f : 0.f;
	
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(BaseMesh->GetRelativeRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 3.f);

	// Yaw
	YawMove = FMath::FInterpTo(YawMove,  Value * GetWorld()->GetDeltaSeconds() * 150.f, GetWorld()->GetDeltaSeconds(), 8.f);
}

void APlayerShipPhysics::CameraYaw(const float Value)
{
	/*// Should reset the camera target if the camera timer is over x seconds
	const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;*/
	
	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	//SpringArmRotTarget.Yaw = bShouldReset ? 0.f : FMath::Clamp(BackSpringArm->GetRelativeRotation().Yaw + Value * 5.f, -80.f, 80.f);
	//SpringArmRotTarget.Yaw = bShouldReset ? 0.f : (BackSpringArm->GetRelativeRotation().Yaw + Value * 5.f);
	SpringArmLocalChange.Yaw = Value;
	
}

void APlayerShipPhysics::CameraPitch(const float Value)
{
	// Should reset the camera target if the camera timer is over x seconds
	//const bool bShouldReset = CameraCenteringTimer >= CameraResetTime;

	if (Value) { CameraCenteringTimer = 0.f; }

	// Target camera rotation should be 0 if bShouldReset is true
	//SpringArmRotTarget.Pitch = bShouldReset ? -20.f : FMath::Clamp(BackSpringArm->GetRelativeRotation().Pitch + Value * 10.f, -85.f, 20.f);
	SpringArmLocalChange.Pitch = Value;
}

void APlayerShipPhysics::CameraUpdate()
{
	/** Springarm rotation */
	if (CameraCenteringTimer >= CameraResetTime)
	{
		ActiveSpringArm->SetRelativeRotation(FMath::RInterpTo(ActiveSpringArm->GetRelativeRotation(), InitialBackSpringArmRotation, GetWorld()->GetDeltaSeconds(), 5.f));
	}
	else
	{
		if(ActiveSpringArm->GetRelativeRotation().Pitch + SpringArmLocalChange.Pitch > 30)
		{
			SpringArmLocalChange.Pitch = 30 - ActiveSpringArm->GetRelativeRotation().Pitch;
		}
		else if (ActiveSpringArm->GetRelativeRotation().Pitch + SpringArmLocalChange.Pitch < -80)
		{
			SpringArmLocalChange.Pitch = -80 - ActiveSpringArm->GetRelativeRotation().Pitch;
		}
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

void APlayerShipPhysics::Shoot(const float Value)
{
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
	/*else if (GunClickSound) 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GunClickSound, 0.8f);
	}*/
}

void APlayerShipPhysics::Reload()
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

void APlayerShipPhysics::Dash() 
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

void APlayerShipPhysics::Jump()
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

void APlayerShipPhysics::Crouch()
{
	TargetHeight /= 2.f;
}

void APlayerShipPhysics::CrouchEnd()
{
	TargetHeight *= 2.f;
}

void APlayerShipPhysics::CameraZoomIn()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength - 200.f, 200.f, 5000.f);
}

void APlayerShipPhysics::CameraZoomOut()
{
	if (bIsDashing) { return; }
	TargetSpringArmLength = FMath::Clamp(TargetSpringArmLength + 200.f, 200.f, 5000.f);
}

void APlayerShipPhysics::SloMo(float Amount, float Duration)
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

void APlayerShipPhysics::CameraSwap()
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
		ForwardsSpeed = 500.f;
	}
	else
	{
		Root->SetLinearDamping(InitialLinearDamping);	
		ForwardsSpeed = 5500.f;
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
	
	// Get distance from thrust point to ground - Equation credit: Anders Åsbø
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
