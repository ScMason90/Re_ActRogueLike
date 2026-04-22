// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AR_BTService_CheckLowHealth.generated.h"

struct FBTServiceLowHealthMemory
{
	// Last trigger time
	float LastTriggerTime = -1000.0f;
};	

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTService_CheckLowHealth : public UBTService
{
	GENERATED_BODY()
	
public:
	UAR_BTService_CheckLowHealth();

protected:
	// We should introduce a struct and override GetNodeMemory for every actor using shared BT asset during runtime 
	
	// Residual blood threshold (percentage)
	UPROPERTY(EditAnywhere, Category="AI")
	float LowHealthThreshold = 0.3f;

	// Cooldown time (seconds)
	UPROPERTY(EditAnywhere, Category="AI")
	float CooldownTime = 60.0f;

	// Blackboard key: Whether to run away
	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector ShouldFleeKey;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
public:
	virtual uint16 GetInstanceMemorySize() const override;
	
};
