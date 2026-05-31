// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_CoinPickupSubsystem.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"

void UAR_CoinPickupSubsystem::AddCoinPickups(TArray<FVector> NewLocations, TArray<int32> NewAmounts)
{
	
	
	CoinLocations.Append(NewLocations);
	CoinAmounts.Append(NewAmounts);
}

void UAR_CoinPickupSubsystem::RemoveCoinPickup(int32 IndexToRemove)
{
	CoinLocations.RemoveAt(IndexToRemove);
	CoinAmounts.RemoveAt(IndexToRemove);
}

void UAR_CoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UWorld* World = GetWorld();
	
	// This is ambiguous when multiplay/network(players).
	FVector PlayerLocation = FVector::ZeroVector;
	for (AAR_PlayerCharacter* PlayerCharacter : TActorRange<AAR_PlayerCharacter>(World))
	{
		PlayerLocation = PlayerCharacter->GetActorLocation();
	}
	
	const float PickupRadius = 200.0f;
	TArray<int32> ProcessList;
	
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		const float Dist = FVector::Dist(PlayerLocation, CoinLocations[i]);
		if (Dist < PickupRadius) ProcessList.Add(i);
	}
	
	int32 TotalCoinsToGrant = 0;
	for (int i = ProcessList.Num() - 1; i >= 0; --i)
	{
		int32 CoinIndex = ProcessList[i];
		TotalCoinsToGrant += CoinAmounts[CoinIndex];
		
		RemoveCoinPickup(CoinIndex);
	}
	
	// TODO: grant coins to player(s)
	UE_CLOG(TotalCoinsToGrant > 0, LogGame, Log, 
		TEXT("UAR_CoinPickupSubsystem::Tick, Picked up Coin Amount=%d"), TotalCoinsToGrant);
	
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(World, CoinLocations[i], 8.0f, FColor::White);
	}
}
