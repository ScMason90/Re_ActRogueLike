// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_CoinPickupSubsystem.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Components/AudioComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Core/AR_DeveloperSettings.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"
#include "Sound/SoundBase.h"

void UAR_CoinPickupSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	UWorld* World = GetWorld();
	
	WorldISM = NewObject<UInstancedStaticMeshComponent>(World, NAME_None, RF_Transient);
	WorldISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WorldISM->RegisterComponentWithWorld(World);
	
	WorldAudioComponent = NewObject<UAudioComponent>(World, NAME_None, RF_Transient);
	WorldAudioComponent->SetAutoActivate(false);
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
}

void UAR_CoinPickupSubsystem::RemoveCoinPickup(int32 IndexToRemove)
{
	CoinLocations.RemoveAt(IndexToRemove);
	CoinAmounts.RemoveAt(IndexToRemove);
	
	WorldISM->RemoveInstanceById(MeshIDs[IndexToRemove]);
	MeshIDs.RemoveAt(IndexToRemove);
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
	
	if (TotalCoinsToGrant > 0)
	{
		PlayPickupSound();
	}
	
	// TODO: grant coins to player(s)
	UE_CLOG(TotalCoinsToGrant > 0, LogGame, Log, 
		TEXT("UAR_CoinPickupSubsystem::Tick, Picked up Coin Amount = %d"), TotalCoinsToGrant);
	
	for (int i = 0; i < CoinLocations.Num(); ++i)
	{
		DrawDebugPoint(World, CoinLocations[i], 8.0f, FColor::White);
	}
}
