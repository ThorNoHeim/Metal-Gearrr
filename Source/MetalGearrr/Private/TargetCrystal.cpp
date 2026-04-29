// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetCrystal.h"

#include "GeometryCollection/GeometryCollectionActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

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
	
	OnTakeAnyDamage.AddDynamic(this, &ATargetCrystal::OnAnyDamage);
	
}

void ATargetCrystal::OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (InstigatedBy && InstigatedBy->IsPlayerController())
	{
		const FTransform Transform = FTransform(GetActorLocation());
		
		if (const AGeometryCollectionActor* GeoActor = GetWorld()->SpawnActor<AGeometryCollectionActor>(AGeometryCollectionActor::StaticClass(), Transform))
		{	
			UGeometryCollectionComponent* GeoCollection = GeoActor->GetGeometryCollectionComponent();
			
			GeoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
			GeoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
			GeoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle, ECollisionResponse::ECR_Ignore);
			GeoCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
			
			GeoCollection->SetRestCollection(GeometryCollection);
			
			GeoCollection->CrumbleActiveClusters();
			GeoCollection->AddRadialImpulse(GetActorLocation(), 200.f, 600.f, RIF_Linear, true);
			
		}
		
		Destroy();
	}
	
}

