// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "AR_BTDecorator_IsLowHealthAndCoolDown.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTDecorator_IsLowHealthAndCoolDown : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UAR_BTDecorator_IsLowHealthAndCoolDown();
	
protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	float CoolDownTime = 60.0f;	// In seconds...
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float LowHealthThreshold = 0.3f;	// Editable in UE editor
	
	UPROPERTY()
	float LastTriggerTime = -1000.0f; // GetWorld - TimeSeconds()...
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
