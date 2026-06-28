// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"
#include "AR_PrimaryGameMode.generated.h"


struct FAR_DirectorData;

namespace EEnvQueryStatus
{
	enum Type : int;
}
struct FEnemySpawnData;
struct FEnvQueryResult;
class UCurveFloat;
class UDataTable;
class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;

/** TODO: Legacy CreditSpawnSystem has not been restructured or cleaned up yet. - 2026.6.25 17:07 
 * you can refer to the new 'SpawnEnemy' framework and combine it with 'ActionSystem' to complete refactor.
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PrimaryGameMode : public AAR_GameModeBase
{
	GENERATED_BODY()
	
public:
	
	AAR_PrimaryGameMode();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void StartPlay() override;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	TArray<FAR_DirectorData> Directors;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	int32 GlobalStartingSeed = 0;	// For generate a global 'FRandomStream'
	
	void SpawnEnemyQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult, FEnemySpawnData* SelectedEnemy);
	
	void OnEnemyClassLoaded(const FSoftObjectPath& LoadedObjectPath, UObject* LoadedObject, FVector SpawnLocation, FEnemySpawnData* SelectedEnemy);
	
	bool TrySpawnEnemy(FAR_DirectorData& Director);

	/* ------- Legacy ------- */

	UPROPERTY(EditDefaultsOnly, Category = "Legacy Spawn System | Enemy")
	TObjectPtr<UCurveFloat> DifficultyCurve;
	
	UFUNCTION()
	void OnPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AR Player State Relative")
	int32 CreditsPerKill;
	
	UPROPERTY(EditDefaultsOnly, Category = "Legacy Spawn System | Pickup")
	TObjectPtr<UEnvQuery> PickupSpawnQuery;
	
	/** All pick-up classes used to spawn with EQS at match start */
	UPROPERTY(EditDefaultsOnly, Category = "Legacy Spawn System | Pickup")
	TArray<TSubclassOf<AActor>> PickupClasses;
	
	/** Distance required between pick-up spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Legacy Spawn System | Pickup")
	float RequiredPickupDistance;
	
	/** Amount of pickups to spawn during match start 
	 * Please note that this amount should be larger than actual 'QueryResultLocations' amount of EQS.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Legacy Spawn System | Pickup")
	int32 DesiredPickupCount;
	
	/** Kills all actors of a specific class - Exposed to Exec and Blueprint */
	UFUNCTION(Exec, BlueprintCallable, Category = "Debug")
	void KillAllOfClass(TSubclassOf<AActor> ClassToKill);

	/** Template version for convenient C++ usage */
	template<typename T>
	void KillAllOfClass() {KillAllOfClass(T::StaticClass());}   // Forwarding Call
	
	UFUNCTION()	
	void RespawnPlayerElapsed(AController* Controller);

	/** @deprecated This isn't working for now in new our own GAS framework - ActionSystem (26.5.24)
	 * @bug - PlayerHealthBar won't sync the new respawn player 'Health'. */
	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);
};
