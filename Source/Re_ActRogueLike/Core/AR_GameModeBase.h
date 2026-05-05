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
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UCurveFloat> DifficultyCurve;

	FTimerHandle TimerHandle_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
public:
	/** Kills all actors of a specific class - Exposed to Exec and Blueprint */
	UFUNCTION(Exec, BlueprintCallable, Category = "Debug")
	void KillAllOfClass(TSubclassOf<AActor> ClassToKill);

	/** Template version for convenient C++ usage */
	template<typename T>
	void KillAllOfClass() {KillAllOfClass(T::StaticClass());}   // Forwarding Call

};
// TODO: Player Respawn Mechanic?Follow TomLooman's Lec15.