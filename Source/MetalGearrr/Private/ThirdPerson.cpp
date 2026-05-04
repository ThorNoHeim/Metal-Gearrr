// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPerson.h"

#include "AmmoChanged.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystemInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SnakeAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AThirdPerson::AThirdPerson()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AThirdPerson::BeginPlay()
{
	Super::BeginPlay();

	//History of me creating a mapping context but abandoning it
	/*if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer:: GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				Subsystem->AddMappingContext(MappingContext, 0);
				//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
					
						//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AThirdPerson::PlayerJump);
						//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPerson::PlayerMove);
						//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPerson::PLayerLook);
	
					

				
			}
		}
	}

*/
	//


	if (!CameraBoomRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: CameraBoomRef not set"));
	}

	if (!FollowCameraRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: FollowCameraRef not set"));
	}

	AnimInst = Cast<USnakeAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInst)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: Failed to get SnakeAnimInstance"));
	}
}

// Called every frame
void AThirdPerson::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimTick(DeltaTime);

	// Check for safety
	if (SpawnedGun && !bIsAiming)
	{
		SpawnedGun->Destroy();
	}
}

// Called to bind functionality to input
void AThirdPerson::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Sneak
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Triggered, this, &AThirdPerson::StartSneak);
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Completed, this, &AThirdPerson::StopSneak);

		// Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AThirdPerson::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AThirdPerson::StopAim);

		// Shoot
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AThirdPerson::StartShoot);
		// Move
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPerson::Move);

		// Look
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPerson::Look);

		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AThirdPerson::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AThirdPerson:StopJumping);

		// Grenade
		EnhancedInputComponent->BindAction(HeldGrenadeAction, ETriggerEvent::Started, this,
		                                   &AThirdPerson::OnGrenadePressed);
	}
}

// Sneak
void AThirdPerson::StartSneak() { Crouch(); }

void AThirdPerson::StopSneak() { UnCrouch(); }

void AThirdPerson::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (AnimInst)
	{
		AnimInst->bIsCrouching = true;
	}
}

void AThirdPerson::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	if (AnimInst)
	{
		AnimInst->bIsCrouching = false;
	}
}

// Aim
void AThirdPerson::StartAim()
{
	UpdateAimSettings(true, false, true, false, 2.8, 1000, 0);

	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: GetWorld() is NULL"));
		return;
	}

	if (!GunActor)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: GunActor is NULL"));
		return;
	}

	// Spawn the gun
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FVector Location = FVector::ZeroVector;
	const FRotator Rotation = FRotator::ZeroRotator;
	const FVector Scale = FVector(1.2f, 1.2f, 1.2f);

	const FTransform SpawnTransform(Rotation, Location, Scale);

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(GunActor, SpawnTransform, SpawnParams);
	if (!SpawnedActor)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: Failed to spawn GunActor"));
		return;
	}

	SpawnedGun = SpawnedActor;

	UMeshComponent* FoundMesh = SpawnedGun->FindComponentByClass<UMeshComponent>();
	if (!FoundMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: SpawnedGun has no MeshComponent"));
		return;
	}

	// Attach the gun
	const FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);

	SpawnedGun->AttachToComponent(GetMesh(), AttachmentRules, TEXT("RightHandSocket"));

	// Save muzzle location
	MuzzleLocation = FoundMesh->GetSocketLocation(TEXT("Muzzle"));
}

void AThirdPerson::StopAim()
{
	UpdateAimSettings(false, true, false, true, 4.0, 0, 500);

	if (SpawnedGun)
	{
		SpawnedGun->Destroy();
		SpawnedGun = nullptr;
	}
}

void AThirdPerson::UpdateAimSettings(bool bAiming, bool OrientRotation, bool UseYaw, bool DoCollision, float FStop,
                                     int32 FocalDistance, int32 FarRegion)
{
	if (!CameraBoomRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: CameraBoomRef is NULL"));
		return;
	}

	if (!FollowCameraRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: FollowCameraRef is NULL"));
		return;
	}

	bIsAiming = bAiming;
	GetCharacterMovement()->bOrientRotationToMovement = OrientRotation;
	bUseControllerRotationYaw = UseYaw;
	CameraBoomRef->bDoCollisionTest = DoCollision;

	// Apply follow camera settings
	FPostProcessSettings& FollowCameraSettings = FollowCameraRef->PostProcessSettings;
	FollowCameraSettings.bOverride_DepthOfFieldFstop = true;
	FollowCameraSettings.bOverride_DepthOfFieldFocalDistance = true;
	FollowCameraSettings.bOverride_DepthOfFieldFarTransitionRegion = true;

	FollowCameraSettings.DepthOfFieldFstop = FStop;
	FollowCameraSettings.DepthOfFieldFocalDistance = FocalDistance;
	FollowCameraSettings.DepthOfFieldFarTransitionRegion = FarRegion;
}

void AThirdPerson::UpdateAimTick(float DeltaTime) const
{
	if (!AnimInst)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: AnimInst is NULL"));
		return;
	}

	if (!CameraBoomRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: CameraBoomRef is NULL"));
		return;
	}

	AnimInst->bIsAiming = bIsAiming;
	AnimInst->CharacterPitch = FMath::Clamp(FRotator::NormalizeAxis(GetControlRotation().Pitch), -70.0f, 70.0f) * -1;

	if (bIsAiming)
	{
		UpdateCamera(
			DeltaTime,
			GetMesh()->GetSocketLocation("CameraAim"),
			FVector::ZeroVector,
			0.0f,
			50.0f,
			100.0f
		);
	}
	else
	{
		FVector TargetLocation = FVector::ZeroVector;
		FVector SocketLocation = GetMesh()->GetSocketLocation("CameraDefault");

		if (IsCrouched())
		{
			SocketLocation.Z /= 1.5;
			TargetLocation = SocketLocation;
		}
		else
		{
			TargetLocation = SocketLocation;
		}

		UpdateCamera(
			DeltaTime,
			FMath::VInterpTo(CameraBoomRef->GetComponentLocation(), TargetLocation, DeltaTime, 4.0f),
			{0.0f, 50.0f, 0.0f},
			400.0f,
			90.0f,
			6.0f
		);
	}
}

void AThirdPerson::UpdateCamera(float DeltaTime, FVector CameraLocation, FVector SocketOffset, float ArmLength,
                                float FieldOfView, float LagSpeed) const
{
	if (!CameraBoomRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: CameraBoomRef is NULL"));
		return;
	}

	if (!FollowCameraRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: FollowCameraRef is NULL"));
		return;
	}

	CameraBoomRef->SetWorldLocation(CameraLocation, true);
	CameraBoomRef->TargetArmLength = FMath::FInterpTo(CameraBoomRef->TargetArmLength, ArmLength, DeltaTime, 5.0);
	CameraBoomRef->SocketOffset = FMath::VInterpTo(CameraBoomRef->SocketOffset, SocketOffset, DeltaTime, 10.0f);
	CameraBoomRef->CameraLagSpeed = FMath::FInterpTo(CameraBoomRef->CameraLagSpeed, LagSpeed, 5.0, 2.0f);

	FollowCameraRef->SetFieldOfView(
		FMath::FInterpConstantTo(FollowCameraRef->FieldOfView, FieldOfView, DeltaTime, 100.0f));
}

// Shoot
void AThirdPerson::StartShoot()
{
	if (!GetWorld())
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: GetWorld() is NULL"));
		return;
	}

	if (!AnimInst)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: AnimInst is NULL"));
		return;
	}

	if (!SpawnedGun)
	{
		UE_LOG(LogTemp, Error, TEXT("AThirdPerson: SpawnedGun is NULL"));
		return;
	}

	if (bIsAiming && bCanShoot && CurrentAmmo > 0)
	{
		bCanShoot = false;

		PlayAnimMontage(SnakeShootMontage);

		AnimInst->bHasShot = true;

		if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			// Get center of the screen
			int32 ViewportX;
			int32 ViewportY;

			PlayerController->GetViewportSize(ViewportX, ViewportY);

			FVector WorldLocation;
			FVector WorldDirection;

			PlayerController->DeprojectScreenPositionToWorld(ViewportX / 2, ViewportY / 2, WorldLocation,
			                                                 WorldDirection);

			// Trace from screen center
			FHitResult HitResultMiddle;
			const bool bHitMiddle = GetWorld()->LineTraceSingleByChannel(
				HitResultMiddle,
				WorldLocation + (WorldDirection * 50),
				WorldLocation + (WorldDirection * 10000),
				ECC_Visibility
			);

			MuzzleLocation = SpawnedGun->FindComponentByClass<UMeshComponent>()->GetSocketLocation(TEXT("Muzzle"));

			// Trace from muzzle
			FHitResult HitResultMuzzle;
			GetWorld()->LineTraceSingleByChannel(
				HitResultMuzzle,
				MuzzleLocation,
				HitResultMiddle.ImpactPoint - (HitResultMiddle.ImpactNormal * 10),
				ECC_Visibility
			);

			// Apply damage
			UGameplayStatics::ApplyDamage(
				HitResultMuzzle.GetActor(),
				GunDamage,
				GetController(),
				this,
				UDamageType::StaticClass()
			);

			// Spawn bullet trail
			UNiagaraComponent* NiagaraTrail = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NSTrail,
				MuzzleLocation
			);

			if (NiagaraTrail)
			{
				NiagaraTrail->SetVariableVec3(TEXT("BeamStart"), MuzzleLocation);
				if (bHitMiddle)
				{
					NiagaraTrail->SetVariableVec3(TEXT("BeamEnd"), HitResultMuzzle.Location);

					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						NSImpact,
						HitResultMuzzle.Location,
						HitResultMuzzle.ImpactNormal.Rotation()
					);
				}
				else
				{
					// Nothing hit, trail extends to max range
					NiagaraTrail->SetVariableVec3(TEXT("BeamEnd"), WorldLocation + (WorldDirection * 10000));
				}
			}

			IAmmoChanged::Execute_AmmoChange(this, -1);

			OnAmmoSpent.Broadcast();

			// Play sound
			if (GunFire)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					GunFire,
					GetActorLocation()
				);
			}

			// Start shoot cooldown
			GetWorldTimerManager().SetTimer(
				TimerHandle_ShootCooldown,
				this,
				&AThirdPerson::ResetCooldown,
				GunCooldown,
				false
			);
		}
	}
}

void AThirdPerson::ResetCooldown()
{
	bCanShoot = true;
}

//For movement
void AThirdPerson::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// Find direction based on where you look
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

//For Looking
void AThirdPerson::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void AThirdPerson::OnGrenadePressed()
{
	if (BP_Grenade && GrenadeThrowMontage)
	{
		PlayAnimMontage(GrenadeThrowMontage);
		FVector SpawnLocation = GetMesh()->GetSocketLocation("LeftHandGrenadeSocket");
		FRotator SpawnRotation = GetMesh()->GetSocketRotation("LeftHandGrenadeSocket");
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		HeldGrenade = GetWorld()->SpawnActor<AActor>(BP_Grenade, SpawnLocation, SpawnRotation, SpawnParams);

		if (HeldGrenade)
		{
			HeldGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                               "LeftHandGrenadeSocket");


			float ThrowDelay = 2.0f;
			FTimerHandle ThrowTimerHandle;
			GetWorldTimerManager().SetTimer(ThrowTimerHandle, this, &AThirdPerson::ExecuteGrenadeThrow, ThrowDelay,
			                                false);
		}
	}
}

void AThirdPerson::ExecuteGrenadeThrow()
{
	if (HeldGrenade)
	{
		HeldGrenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(HeldGrenade->GetRootComponent());
		if (RootComp)
		{
			RootComp->SetSimulatePhysics(true);
			RootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			FVector LaunchDirection = GetControlRotation().Vector();
			FVector Velocity = (LaunchDirection + FVector(0, 0, 0.2f)) * 1500.f;

			RootComp->AddImpulse(Velocity, NAME_None, true);
		}
		HeldGrenade = nullptr;
	}
}


/*void AThirdPerson::OnGrenadePressed()
{
	if (BP_Grenade && GrenadeDrop)
	{
		PlayAnimMontage(GrenadeDrop);


		FVector SpawnLocation = GetMesh()->GetSocketLocation("LeftHandGrenadeSocket");
		FRotator SpawnRotation = GetMesh()->GetSocketRotation("LeftHandGrenadeSocket");

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();


		HeldGrenade = GetWorld()->SpawnActor<AActor>(BP_Grenade, SpawnLocation, SpawnRotation, SpawnParams);

		if (HeldGrenade)
		{
			HeldGrenade->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
			                               "LeftHandGrenadeSocket");
		}
	}
}
/*
/*void AThirdPerson::ReleaseGrenade()
{
	if (HeldGrenade)
	{
		HeldGrenade->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(HeldGrenade->GetRootComponent());
		if (RootComp)
		{
			RootComp->SetSimulatePhysics(true);
			RootComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			FVector LaunchDirection = GetControlRotation().Vector();
			FVector Velocity = (LaunchDirection + FVector(0, 0, 0.2f)) * 1500.f;

			RootComp->AddImpulse(Velocity, NAME_None, true);
		}


		HeldGrenade = nullptr;
	}
}
*/
