
#pragma once

#include "Re_ActRoguelikeTypes.generated.h"

#define NAME_TargetActor "TargetActor"

// For some legacy config(in DefaultEngine.ini file), ECC_GameTraceChannel in this project was kind weird
#define	COLLISION_INTERACTION ECC_GameTraceChannel3
#define COLLISION_PROJECTILE ECC_GameTraceChannel1


class AAR_AICharacter;

USTRUCT()
struct FEnemySpawnData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AAR_AICharacter> EnemyClass;
	
	/* Points required by GameMode to spawn this unit.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCosts = 0.0f;
	
};
