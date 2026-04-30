// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SnakeAnimInstance.generated.h"

UCLASS()
class USnakeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bHasShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool bIsAiming = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float CharacterPitch;
};
