// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BasePickUp.generated.h"

UCLASS()
class ABasePickUp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePickUp();

	UFUNCTION()
	virtual void BindEvent();

	UFUNCTION()
	virtual void UnbindEvent();

	UFUNCTION()
	virtual void TryPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class AThirdPerson* OverlappingPlayer;

	// Overlap functions
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

private:
	// Set reference to player
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ACharacter> PlayerClassReference;
};
