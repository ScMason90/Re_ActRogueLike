// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "AR_BTDecorator_IsLowHealthAndCoolDown.generated.h"

struct FBTDecoratorLowHealthMemory
{
	float LastTriggerTime = -1000.0f; // GetWorld - TimeSeconds()...
};

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTDecorator_IsLowHealthAndCoolDown : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	
	UAR_BTDecorator_IsLowHealthAndCoolDown();
	
	virtual uint16 GetInstanceMemorySize() const override;
	
protected:
	// We should introduce a struct and override GetNodeMemory for every actor using shared BT asset during runtime 
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float CoolDownTime = 60.0f;	// In seconds...
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float LowHealthThreshold = 0.3f;	// Editable in UE editor
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
