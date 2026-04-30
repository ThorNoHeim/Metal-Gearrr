// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickUp.h"

#include "HealthChanged.h"
#include "ThirdPerson.h"

// Sets default values
AHealthPickUp::AHealthPickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AHealthPickUp::BeginPlay()
{
	Super::BeginPlay();
}

void AHealthPickUp::BindEvent()
{
	OverlappingPlayer->OnDamageTaken.AddUniqueDynamic(this, &ABasePickUp::TryPickUp);
}

void AHealthPickUp::UnbindEvent()
{
	OverlappingPlayer->OnDamageTaken.RemoveDynamic(this, &ABasePickUp::TryPickUp);
}

void AHealthPickUp::TryPickUp()
{
	// Ensure overlapping player is set
	if (!OverlappingPlayer)
		return;

	// Only pick up if the player isn't already at max health
	if (OverlappingPlayer->CurrentHealth + 1 <= OverlappingPlayer->MaxHealth)
	{
		// Unbind event
		UnbindEvent();

		// Apply health
		if (OverlappingPlayer->GetClass()->ImplementsInterface(UHealthChanged::StaticClass()))
		{
			IHealthChanged::Execute_HealthChange(OverlappingPlayer, 1);
		}

		// Hide and disable the pickup, then destroy it
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(0.01f);
	}
}
