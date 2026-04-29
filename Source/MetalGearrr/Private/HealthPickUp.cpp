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

void AHealthPickUp::TryPickUp()
{
	if (OverlappingPlayer->CurrentHealth + 1 <= OverlappingPlayer->MaxHealth)
	{
		if (OverlappingPlayer) 
		{
			OverlappingPlayer->OnDamageTaken.RemoveDynamic(this, &ABasePickUp::TryPickUp);
			
			if (OverlappingPlayer->GetClass()->ImplementsInterface(UHealthChanged::StaticClass()))
			{
				IHealthChanged::Execute_HealthChange(OverlappingPlayer, 1);
			}
		
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		
			SetLifeSpan(0.01f);
		}
	}
}

