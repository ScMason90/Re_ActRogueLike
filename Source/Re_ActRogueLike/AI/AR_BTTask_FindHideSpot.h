// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AR_BTTask_FindHideSpot.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}
struct FEnvQueryResult;
class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_BTTask_FindHideSpot : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UAR_BTTask_FindHideSpot();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UEnvQuery> QueryTemplate;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector HideLocationKey;
	
	UFUNCTION()
	void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UBehaviorTreeComponent* CachedOwnerComp = nullptr;
};
