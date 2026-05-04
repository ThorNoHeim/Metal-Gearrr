// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "TargetCrystal.generated.h"

UCLASS()
class ATargetCrystal : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATargetCrystal();
	
	// Sounds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* CrystalShatter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// When Crystal takes any damage
	UFUNCTION()
	void OnAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	                 AActor* DamageCauser);

private:
	// Set Geometry Collection
	UPROPERTY(EditDefaultsOnly, Category = "Geometry")
	TObjectPtr<UGeometryCollection> GeometryCollection;
};
