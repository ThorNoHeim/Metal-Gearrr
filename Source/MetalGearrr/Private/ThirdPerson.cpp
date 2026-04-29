// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPerson.h"

// Sets default values
AThirdPerson::AThirdPerson()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AThirdPerson::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThirdPerson::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AThirdPerson::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

