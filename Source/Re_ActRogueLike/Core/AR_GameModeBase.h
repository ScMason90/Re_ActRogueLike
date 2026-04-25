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

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionRangedClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UEnvQuery> SpawnBotQuery;

	/*@bugs: idk why i can't assign my new created 'DifficultyCurve' BP asset in ue editor here for this member variable
	 * Tom didn't add all of these lec12 code in ue5.6-course-project branch.He just simply ignored.All logic of bot 
	 * spawning in 'GameMode' class could be done somewhere else.Also, the 'DifficultyCurve' which control spawning bot 
	 * numbers as game time increasing could be replaced by other ways to implement. */
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
	virtual void StartPlay() override;

};
