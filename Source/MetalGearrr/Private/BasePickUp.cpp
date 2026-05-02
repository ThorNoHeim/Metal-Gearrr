// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"

#include "PropertyAccess.h"
#include "ThirdPerson.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ABasePickUp::ABasePickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

// Called when the game starts or when spawned
void ABasePickUp::BeginPlay()
{
	Super::BeginPlay();
	
	if (USphereComponent* FoundSphere = FindComponentByClass<USphereComponent>())
	{
		FoundSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::OnBeginOverlap);
		FoundSphere->OnComponentEndOverlap.AddDynamic(this, &ABasePickUp::OnOverlapEnd);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Couldn't find a Sphere Component"));
	}
}

void ABasePickUp::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && PlayerClassReference)
	{
		if (bool bIsPlayer = OtherActor->IsA(PlayerClassReference))
		{
			OverlappingPlayer = Cast<AThirdPerson>(OtherActor);
			
			TryPickUp();
			
			if (OverlappingPlayer)
			{
				OverlappingPlayer->OnDamageTaken.AddUniqueDynamic(this, &ABasePickUp::TryPickUp);
			}
		}
	}
	else if (!PlayerClassReference)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: PlayerClassReference is NULL"));
	}
}

void ABasePickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappingPlayer)
	{
		OverlappingPlayer->OnDamageTaken.RemoveDynamic(this, &ABasePickUp::TryPickUp);
		OverlappingPlayer = nullptr;
	}
}

void ABasePickUp::TryPickUp()
{
}
