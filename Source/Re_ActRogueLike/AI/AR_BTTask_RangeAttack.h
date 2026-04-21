// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AR_BTTask_RangeAttack.generated.h"

class AAR_MagicProjectile;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTTask_RangeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
protected:
	// We should introduce a struct and override GetNodeMemory for every actor using shared BT asset during runtime 
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;
 
	UPROPERTY(EditAnywhere, Category = "AI")
	FName MuzzleSocketName;
	
	// Misc projectile class?
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AAR_MagicProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread = 5.0f;
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UAR_BTTask_RangeAttack();
};
