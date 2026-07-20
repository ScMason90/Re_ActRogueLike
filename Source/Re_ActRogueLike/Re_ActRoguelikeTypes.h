
#pragma once

#include "Re_ActRoguelikeTypes.generated.h"

#define NAME_TargetActor "TargetActor"

// For some legacy config(in DefaultEngine.ini file), ECC_GameTraceChannel in this project was kind weird
#define	COLLISION_INTERACTION ECC_GameTraceChannel3
#define COLLISION_PROJECTILE ECC_GameTraceChannel1


class UAR_EnemyData;
class UEnvQuery;
class UDataTable;
class AAR_AICharacter;

USTRUCT()
struct FEnemySpawnData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAR_EnemyData> EnemyData;
	
	/* Points required by GameMode to spawn this unit.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0f))
	float SpawnCosts = 0.0f;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.0f))
	float SpawnWeight = 1.0f;
	
};

USTRUCT(BlueprintType)
struct FAR_DirectorData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Enemy")
	TObjectPtr<UEnvQuery> SpawnEnemyLocationQuery;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Enemy")
	TObjectPtr<UDataTable> EnemySpawnTable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System | Credit")
	FRuntimeFloatCurve CreditGainCurve;
	
	float CurrentCredits = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	float TickInterval = 0.0f;
	
	float NextTickTime = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	float TimeBetweenWaves = 6.0f;
	
	FRandomStream RandomStream_EnemySelection;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	FString DebugDisplayName = TEXT("DirectorNameDefault");
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawn System")
	FColor DebugColor = FColor::White;
	
	// Prefix array and binary search optimization for calculating 'SelectedRow' in AAR_PrimaryGameMode::TrySpawnEnemy
	
	UPROPERTY(Transient)
	float TotalSpawnWeight;	// Total quantity of 'SpawnWeight'(in FEnemySpawnData row) within 'EnemySpawnTable'
	
	TArray<float> PrefixWeights;	// Each index mapping prefix(0-index) accumulated SpawnCost of rows of 'EnemySpawnTable'
	
	TArray<FEnemySpawnData*> CachedRows;	// All rows of 'EnemySpawnTable' in the same parent USTRUCT FAR_DirectorData
	
};
