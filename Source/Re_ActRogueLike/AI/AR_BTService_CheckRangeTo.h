// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AR_BTService_CheckRangeTo.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTService_CheckRangeTo : public UBTService
{
	GENERATED_BODY()
	
public:
	UAR_BTService_CheckRangeTo();
	
protected:
	// We should introduce a struct and override GetNodeMemory for every actor using shared BT asset during runtime 
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector WithinRangeKey;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxAttackRange = 500.0f;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
