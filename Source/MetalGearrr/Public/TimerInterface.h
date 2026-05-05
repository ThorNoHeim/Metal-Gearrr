// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TimerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTimerInterface : public UInterface
{
	GENERATED_BODY()
};

class METALGEARRR_API ITimerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Timer")
	void TimerChange(int32 Amount);
};
