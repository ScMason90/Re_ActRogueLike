// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AR_CoinPickupSubsystem.generated.h"

class UAudioComponent;
struct FPrimitiveInstanceId;
class UInstancedStaticMeshComponent;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_CoinPickupSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UAR_CoinPickupSubsystem, STATGROUP_Tickables);
	};

protected:
	
	void OnPickupMeshLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedObject);
	
	void OnPickupSoundLoadComplete(const FSoftObjectPath& SoftObjectPath, UObject* LoadedObject);
	
	void PlayPickupSound();
	
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> WorldISM;
	
	UPROPERTY()
	TObjectPtr<UAudioComponent> WorldAudioComponent;
	
	/* Cached param from Developer Settings for 'Input' event ... 
	 * Corresponding in specific 'USoundBase' MetaSound asset set as 'WorldAudioComponent'.*/
	FName CoinPickupTriggerParamName;
	
	TArray<FVector> CoinLocations;
	TArray<int32> CoinAmounts;
	TArray<FPrimitiveInstanceId> MeshIDs;
	
public:
	
	void AddCoinPickups(TArray<FVector> NewLocations, TArray<int32> NewAmounts);
	
	void RemoveCoinPickup(int32 IndexToRemove);
	
};
