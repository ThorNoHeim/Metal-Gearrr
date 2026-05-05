// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetCrystal.h"

#include "AmmoChanged.h"
#include "TimerInterface.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATargetCrystal::ATargetCrystal()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATargetCrystal::BeginPlay()
{
	Super::BeginPlay();

	// When Crystal takes any damage
	OnTakeAnyDamage.AddDynamic(this, &ATargetCrystal::OnAnyDamage);
}

void ATargetCrystal::OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                 AController* InstigatedBy, AActor* DamageCauser)
{
	// If damaged by the player
	if (!InstigatedBy || !InstigatedBy->IsPlayerController())
	{
		return;
	}

	// Play sound
	if (CrystalShatter)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			CrystalShatter,
			GetActorLocation()
		);
	}

	// Add time
	if (DamageCauser->GetClass()->ImplementsInterface(UTimerInterface::StaticClass()))
	{
		ITimerInterface::Execute_TimerChange(DamageCauser, -2);
	}

	// Refund ammo
	if (DamageCauser->GetClass()->ImplementsInterface(UAmmoChanged::StaticClass()))
	{
		IAmmoChanged::Execute_AmmoChange(DamageCauser, 1);
	}

	// Get spawn location
	const FTransform Transform = FTransform(GetActorLocation());

	// If spawned
	if (const AGeometryCollectionActor* GeoActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(
		AGeometryCollectionActor::StaticClass(), Transform))
	{
		// if GeometryCollectionComponent exists
		if (UGeometryCollectionComponent* GeoCollection = GeoActor->GetGeometryCollectionComponent())
		{
			// Set destroyed pieces collision to ignore
			GeoCollection->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			GeoCollection->SetCollisionResponseToChannel(ECC_PhysicsBody,
			                                             ECR_Ignore);
			GeoCollection->
				SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
			GeoCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

			// Set the collection
			GeoCollection->SetRestCollection(GeometryCollection);

			// Crumble the collection
			GeoCollection->CrumbleActiveClusters();
			GeoCollection->AddRadialImpulse(GetActorLocation(), 200.f, 600.f, RIF_Linear, true);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ATargetCrystal: GeoActor has no GeometryCollectionComponent"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ATargetCrystal: Failed to spawn GeoActor"));
	}

	Destroy();
}
