// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_CoinTestActor.h"

#include "NavigationSystem.h"
#include "Re_ActRogueLike/Pickups/AR_CoinPickupSubsystem.h"


// Sets default values
AAR_CoinTestActor::AAR_CoinTestActor()
{
	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComp"));
	RootComponent = DefaultSceneComponent;
	
}

void AAR_CoinTestActor::SpawnCoins(int32 SpawnCount)
{
	TArray<FVector> CoinLocations;
	TArray<int32> CoinAmounts;
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	FVector ActorLocation = GetActorLocation();
	
	for (int i = 0; i < SpawnCount; ++i)
	{
		FNavLocation NavLocation;
		NavSystem->GetRandomPointInNavigableRadius(ActorLocation, 1024, NavLocation);
		
		CoinLocations.Add(NavLocation.Location);
		CoinAmounts.Add(10);
	}
	
	UAR_CoinPickupSubsystem* CoinSystem = GetWorld()->GetSubsystem<UAR_CoinPickupSubsystem>();
	
	CoinSystem->AddCoinPickups(CoinLocations, CoinAmounts);
}

