// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AR_BTTask_HealSelf.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTTask_HealSelf : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UAR_BTTask_HealSelf();
	
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
