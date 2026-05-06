// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Grenade.generated.h"

class UNiagaraSystem;

UCLASS()
class AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grenade | Components")
	TObjectPtr<USphereComponent> SphereCollisionComponent;
	 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grenade | Components")
	TObjectPtr<UStaticMeshComponent> GrenadeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Grenade | Components")
	TObjectPtr<URadialForceComponent> RadialForceComponent;

	// Expose the Niagara system to the blueprint
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<UNiagaraSystem> ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade | Explosion")
	float GrenadeDamage = 1000.f;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade | Explosion")
	float ExplosionRadius = 160.f;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Grenade | Explosion")
	float FuseTime = 3.0f;

	FTimerHandle FuseTimerHandle;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Function to be called when grenade is to be exploded.
	void Explode();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Call this to explode the grenade after a delay.
	void PrepareForExplosion();
};
