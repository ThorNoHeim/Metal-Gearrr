// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePickUp.h"
#include "GameFramework/Actor.h"
#include "AmmoPickUp.generated.h"

UCLASS()
class AAmmoPickUp : public ABasePickUp
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BindEvent() override;
	virtual void UnbindEvent() override;

	virtual void TryPickUp() override;
};
