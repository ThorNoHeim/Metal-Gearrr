// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePickUp.h"

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

	// Get collision sphere
	if (USphereComponent* FoundSphere = FindComponentByClass<USphereComponent>())
	{
		FoundSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasePickUp::OnBeginOverlap);
		FoundSphere->OnComponentEndOverlap.AddDynamic(this, &ABasePickUp::OnOverlapEnd);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABasePickUp: Couldn't find a Sphere Component"));
	}
}

void ABasePickUp::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	// Ensure PlayerClassReference is set
	if (!PlayerClassReference)
	{
		UE_LOG(LogTemp, Error, TEXT("ABasePickUp: PlayerClassReference is NULL"));
		return;
	}

	// If other actor is the player
	if (OtherActor && PlayerClassReference)
	{
		if (OtherActor && OtherActor->IsA(PlayerClassReference))
		{
			OverlappingPlayer = Cast<AThirdPerson>(OtherActor);

			// Attempt pickup immediately on overlap
			TryPickUp();

			if (OverlappingPlayer)
			{
				// Bind event so pick up can be retried while overlapping
				BindEvent();
			}
		}
	}
}

void ABasePickUp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Only respond to the currently overlapping player leaving
	if (OtherActor == OverlappingPlayer)
	{
		// Unbind event
		UnbindEvent();
	}
}

void ABasePickUp::BindEvent()
{
}

void ABasePickUp::UnbindEvent()
{
}

void ABasePickUp::TryPickUp()
{
}
