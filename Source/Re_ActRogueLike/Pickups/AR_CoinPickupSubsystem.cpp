// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_CoinPickupSubsystem.h"

#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "ProfilingDebugging/CountersTrace.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_DeveloperSettings.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"


TRACE_DECLARE_INT_COUNTER(CoinInstanceCount, TEXT("Coins in World"));


void UAR_CoinPickupSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UWorld* World = GetWorld();
	
	WorldISM = NewObject<UInstancedStaticMeshComponent>(World, NAME_None, RF_Transient);
	WorldISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WorldISM->SetAffectDistanceFieldLighting(false);
	WorldISM->RegisterComponentWithWorld(World);
	
	TRACE_COUNTER_SET(CoinInstanceCount, 0);
	
	WorldAudioComponent = NewObject<UAudioComponent>(World, NAME_None, RF_Transient);
	WorldAudioComponent->SetAutoActivate(false);
	WorldAudioComponent->bAllowSpatialization = false;
	WorldAudioComponent->RegisterComponentWithWorld(World);
	
	const UAR_DeveloperSettings* DevSettings = GetDefault<UAR_DeveloperSettings>();
	CoinPickupTriggerParamName = DevSettings->CoinPickupTriggerParameter;
	
	DevSettings->CoinPickupMesh.LoadAsync(
		FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &UAR_CoinPickupSubsystem::OnPickupMeshLoadComplete));
	DevSettings->CoinPickupSound.LoadAsync(
		FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, &UAR_CoinPickupSubsystem::OnPickupSoundLoadComplete));
	
}

void UAR_CoinPickupSubsystem::OnPickupMeshLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedObject)
{
	WorldISM->SetStaticMesh(Cast<UStaticMesh>(LoadedObject));
}

void UAR_CoinPickupSubsystem::OnPickupSoundLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedObject)
{
	WorldAudioComponent->SetSound(Cast<USoundBase>(LoadedObject));
}

void UAR_CoinPickupSubsystem::PlayPickupSound()
{
	if (!WorldAudioComponent->IsPlaying())WorldAudioComponent->Play();
	
	WorldAudioComponent->SetTriggerParameter(CoinPickupTriggerParamName);
}

void UAR_CoinPickupSubsystem::AddCoinPickups(TArray<FVector> NewLocations, TArray<int32> NewAmounts)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UAR_CoinPickupSubsystem::AddCoinPickups());
	
	CoinLocations.Append(NewLocations);
	CoinAmounts.Append(NewAmounts);
	
	TArray<FTransform> MeshTransforms;
	for (int i = 0; i <= NewLocations.Num() - 1; ++i)
	{
		MeshTransforms.Add(FTransform(NewLocations[i] + FVector(0.0f, 0.0f, 50.0f)));
	}
	
	TArray<FPrimitiveInstanceId> NewMeshIDs = WorldISM->AddInstancesById(
		MeshTransforms, true, false);
	MeshIDs.Append(NewMeshIDs);
	
	TRACE_COUNTER_SET(CoinInstanceCount, CoinLocations.Num());
}

void UAR_CoinPickupSubsystem::RemoveCoinPickup(int32 IndexToRemove)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UAR_CoinPickupSubsystem::RemoveCoinPickup());
	
#if 1
	// Specified init 'TArray<T, FNonshrinkingAllocator>' to get global shrinking control 
	// instead of manually pass 'EAllowShrinking::No' while execute removal operation
	
	CoinLocations.RemoveAtSwap(IndexToRemove, EAllowShrinking::No);
	CoinAmounts.RemoveAtSwap(IndexToRemove, EAllowShrinking::No);
	
	WorldISM->RemoveInstanceById(MeshIDs[IndexToRemove]);
	MeshIDs.RemoveAtSwap(IndexToRemove, EAllowShrinking::No);
	
#else
	CoinLocations.RemoveAt(IndexToRemove);
	CoinAmounts.RemoveAt(IndexToRemove);
	
	WorldISM->RemoveInstanceById(MeshIDs[IndexToRemove]);
	MeshIDs.RemoveAt(IndexToRemove);
#endif
	
	TRACE_COUNTER_SET(CoinInstanceCount, CoinLocations.Num());
}

void UAR_CoinPickupSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TRACE_CPUPROFILER_EVENT_SCOPE(UAR_CoinPickupSubsystem::Tick());
	
	UWorld* World = GetWorld();
	
	FVector PlayerLocation = FVector::ZeroVector;
	TWeakObjectPtr<UAR_ActionSystemComponent> LocalPlayerASComp = nullptr;
	// This is ambiguous when multiplay/network(players).
	for (AAR_PlayerCharacter* PlayerCharacterIterRange : TActorRange<AAR_PlayerCharacter>(World))
	{
		PlayerLocation = PlayerCharacterIterRange->GetActorLocation();
		LocalPlayerASComp = PlayerCharacterIterRange->GetASComp();
	}
	
	const float PickupRadius = 200.0f;
	TArray<int32> ProcessList;
	
	// Distance Check
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UAR_CoinPickupSubsystem::Tick()::DistanceCheck);
		
		for (int i = 0; i < CoinLocations.Num(); ++i)
		{
			const float Dist = FVector::Dist(PlayerLocation, CoinLocations[i]);
			if (Dist < PickupRadius) ProcessList.Add(i);
		}
	}
	
	int32 TotalCoinsToGrant = 0;
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(UAR_CoinPickupSubsystem::Tick()::HandlePickups);
		
		for (int i = ProcessList.Num() - 1; i >= 0; --i)
		{
			int32 CoinIndex = ProcessList[i];
		
			TotalCoinsToGrant += CoinAmounts[CoinIndex];
			RemoveCoinPickup(CoinIndex);
		}
	}
	
	if (TotalCoinsToGrant > 0)
	{
		PlayPickupSound();
		LocalPlayerASComp.Get()->ApplyAttributeChanged(SharedGameplayTags::Attribute_Credit, TotalCoinsToGrant, Base);
	}
	
#if 0
	// TODO: Multicast RPC(RemoteProcedureCall) and sync 'Spawn/Remove of CoinISM' on server. Server grant players coins by modifying the 'Attribute.Credit' of their ASComp
	
	// Log Who picked and its granted amount if Implement Network Replication on some server?
	UE_CLOG(TotalCoinsToGrant > 0, LogGame, Log, 
		TEXT("UAR_CoinPickupSubsystem::Tick, Local Player picked up Coin Amount = %d"), TotalCoinsToGrant);
	
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(World, CoinLocations[i], 8.0f, FColor::White);
	}
#endif
	
}
