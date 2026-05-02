// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPerson.generated.h"

// Event dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageTaken);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoSpent);

UCLASS()
class AThirdPerson : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThirdPerson();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Blueprint variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxHealth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentHealth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 MaxAmmo = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int32 CurrentAmmo = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	bool bIsAiming = false;

	// Events
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOnDamageTaken OnDamageTaken;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Events")
	FOnAmmoSpent OnAmmoSpent;

	// Input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* SneakAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ShootAction;

	// Camera assets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAssets")
	class USpringArmComponent* CameraBoomRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAssets")
	class UCameraComponent* FollowCameraRef;

	// Gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	TSubclassOf<AActor> GunActor;

	// Animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* SnakeShootMontage;

	// Niagara
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	class UNiagaraSystem* NSTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraSystem* NSImpact;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Crouching
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

private:
	// Gun
	UPROPERTY()
	AActor* SpawnedGun;

	// Animation
	UPROPERTY()
	class USnakeAnimInstance* AnimInst;

	// Variables
	FVector MuzzleLocation;
	FTimerHandle TimerHandle_ShootCooldown;
	bool bCanShoot = true;
	float GunDamage = 1.0f;
	float GunCooldown = 0.5f;

	// Crouch
	void StartSneak();
	void StopSneak();

	// Aim
	void StartAim();
	void StopAim();
	void UpdateAimSettings(bool bAiming, bool OrientRotation, bool UseYaw, bool DoCollision, float FStop,
	                       int32 FocalDistance, int32 FarRegion);
	void UpdateAimTick(float DeltaTime) const;
	void UpdateCamera(float DeltaTime, FVector CameraLocation, FVector SocketOffset, float ArmLength, float FieldOfView,
	                  float LagSpeed) const;

	// Shoot
	void StartShoot();
	void ResetCooldown();
};
