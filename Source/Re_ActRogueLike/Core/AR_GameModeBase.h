// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AR_GameModeBase.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AAR_GameModeBase();
	
	virtual void StartPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionRangedClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UCurveFloat> DifficultyCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;
	
	FTimerHandle TimerHandle_SpawnBots;
	
	UFUNCTION()
	void SpawnBotTimerElapsed();
	UFUNCTION()
	void OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	UFUNCTION()
	void OnPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	int32 CreditsPerKill;
	
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TObjectPtr<UEnvQuery> PickupSpawnQuery;
	
	/** All pick-up classes used to spawn with EQS at match start */
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	TArray<TSubclassOf<AActor>> PickupClasses;
	
	/** Distance required between pick-up spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	float RequiredPickupDistance;
	
	/** Amount of pickups to spawn during match start 
	 * Please note that this amount should be larger than actual 'QueryResultLocations' amount of EQS.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Pickups")
	int32 DesiredPickupCount;
	
	/** Kills all actors of a specific class - Exposed to Exec and Blueprint */
	UFUNCTION(Exec, BlueprintCallable, Category = "Debug")
	void KillAllOfClass(TSubclassOf<AActor> ClassToKill);

	/** Template version for convenient C++ usage */
	template<typename T>
	void KillAllOfClass() {KillAllOfClass(T::StaticClass());}   // Forwarding Call
	
	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);
	
	// @bug - PlayerHealthBar won't sync the respawn player state.
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

};