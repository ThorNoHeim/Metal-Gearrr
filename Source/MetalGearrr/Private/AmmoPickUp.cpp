// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickUp.h"

#include "AmmoChanged.h"
#include "ThirdPerson.h"

// Sets default values
AAmmoPickUp::AAmmoPickUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AAmmoPickUp::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmoPickUp::BindEvent()
{
	OverlappingPlayer->OnAmmoSpent.AddUniqueDynamic(this, &ABasePickUp::TryPickUp);
}

void AAmmoPickUp::UnbindEvent()
{
	OverlappingPlayer->OnAmmoSpent.RemoveDynamic(this, &ABasePickUp::TryPickUp);
}

void AAmmoPickUp::TryPickUp()
{
	if (OverlappingPlayer->CurrentAmmo != OverlappingPlayer->MaxAmmo)
	{
		if (OverlappingPlayer)
		{
			OverlappingPlayer->OnDamageTaken.RemoveDynamic(this, &ABasePickUp::TryPickUp);

			if (OverlappingPlayer->GetClass()->ImplementsInterface(UAmmoChanged::StaticClass()))
			{
				IAmmoChanged::Execute_AmmoChange(OverlappingPlayer, 5);
			}

			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);

			SetLifeSpan(0.01f);
		}
	}
}
