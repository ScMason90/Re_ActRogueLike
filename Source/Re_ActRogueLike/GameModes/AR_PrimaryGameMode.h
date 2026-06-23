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

/** I corrected the file name of my AI behavior tree and the blackboard assets(Minon->Minion), and then 'spawn bots' executed 
 * through 'AR_GameModeBase' will trigger the ensure error of AR AIController.cpp line 25... After checking and attempting 
 * possible minor fixes, it can now be determined that placing 'MinionActor' normally on the level map is harmless. 
 * However, the 'spawn bots' performed will cause severe lag in PIE and trigger callback errors(2026.6.20):
 * LogOutputDevice: Error: === Handled ensure: ===
 * LogOutputDevice: Error: Ensure condition failed: BehaviorTree  [File:D:\Games\Unreal Engine Projects\Re_ActRogueLike\Source\Re_ActRogueLike\AI\AR_AIController.cpp] [Line: 25] 
 * LogOutputDevice: Error: AAR_AIController::BeginPlay(), BehaviorTree is nullptr, please assign it...*/
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
	
	void SpawnEnemyQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult, FEnemySpawnData* SelectedEnemy);
	
	void OnEnemyClassLoaded(const FSoftObjectPath& LoadedObjectPath, UObject* LoadedObject, FVector SpawnLocation, FEnemySpawnData* SelectedEnemy);
	
	bool TrySpawnEnemy(FAR_DirectorData& Director);

	/* ------- Legacy ------- */

	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Enemy")
	TObjectPtr<UCurveFloat> DifficultyCurve;
	
	UFUNCTION()
	void OnPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AR Player State Relative")
	int32 CreditsPerKill;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Pickup")
	TObjectPtr<UEnvQuery> PickupSpawnQuery;
	
	/** All pick-up classes used to spawn with EQS at match start */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Pickup")
	TArray<TSubclassOf<AActor>> PickupClasses;
	
	/** Distance required between pick-up spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Pickup")
	float RequiredPickupDistance;
	
	/** Amount of pickups to spawn during match start 
	 * Please note that this amount should be larger than actual 'QueryResultLocations' amount of EQS.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Pickup")
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
