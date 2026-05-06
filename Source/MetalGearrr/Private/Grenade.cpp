// Fill out your copyright notice in the Description page of Project Settings.

#include "Grenade.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	SphereCollisionComponent->InitSphereRadius(ExplosionRadius);
	SphereCollisionComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = SphereCollisionComponent;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	GrenadeMesh->SetupAttachment(RootComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComponent"));
	RadialForceComponent->SetupAttachment(RootComponent);
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->ImpulseStrength = 100000.f;
	RadialForceComponent->Radius = 300.0f;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (UStaticMesh* LoadedMesh = LoadObject<UStaticMesh>(nullptr, *FString(TEXT("/Game/Grenade/Grenade_LOD01.Grenade_LOD01"))); LoadedMesh && GrenadeMesh)
	{
		GrenadeMesh->SetStaticMesh(LoadedMesh);
		// Optionally set relative location or scale to better fit collision/etc.
		GrenadeMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		GrenadeMesh->SetRelativeScale3D(FVector(0.955f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Uh oh - grenade mesh not loaded or found."));
	}
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenade::PrepareForExplosion()
{
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &AGrenade::Explode, FuseTime,
											false);
	}
}

void AGrenade::Explode()
{
	if (RadialForceComponent)
	{
		RadialForceComponent->FireImpulse();
	}
	
	// Apply radial damage
	UGameplayStatics::ApplyRadialDamage(
		this,
		GrenadeDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(), // Ignore list
		this,
		GetInstigatorController(),
		true // Do full damage
	);

	if (ExplosionVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), 
			ExplosionVFX.Get(), 
			GetActorLocation(), 
			GetActorRotation()
		);
	}

	Destroy();
}

