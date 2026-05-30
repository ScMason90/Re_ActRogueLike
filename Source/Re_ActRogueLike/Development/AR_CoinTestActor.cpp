// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_CoinTestActor.h"


// Sets default values
AAR_CoinTestActor::AAR_CoinTestActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAR_CoinTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAR_CoinTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

