// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AIController.h"


// Sets default values
AAR_AIController::AAR_AIController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAR_AIController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAR_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

