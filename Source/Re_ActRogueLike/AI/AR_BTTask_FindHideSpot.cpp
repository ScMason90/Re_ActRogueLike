// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTTask_FindHideSpot.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UAR_BTTask_FindHideSpot::UAR_BTTask_FindHideSpot()
{
	NodeName = "Find Hide Spot";
}

EBTNodeResult::Type UAR_BTTask_FindHideSpot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !QueryTemplate) return EBTNodeResult::Failed;
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		AIController, QueryTemplate,AIController->GetPawn(), 
		EEnvQueryRunMode::RandomBest5Pct, nullptr);
	
	if (!QueryInstance) return EBTNodeResult::Failed;
	// Save the OwnerComp pointer for callback use
	CachedOwnerComp = &OwnerComp;
	
	// Binding callback functions (using AddDynamic in UE5.7, 'Cause Epic just changed the return type of 'QueryInstance->GetOnQueryFinishedEvent()' 
	// so that we need implemented new API of EQS)
	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UAR_BTTask_FindHideSpot::OnQueryFinished);
	
	return EBTNodeResult::InProgress;
}

void UAR_BTTask_FindHideSpot::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	// Asynchronously check before use (assign in UAR_BTTask_FindHideSpot::ExecuteTask)
	if (!CachedOwnerComp) return;
	
	if (QueryStatus == EEnvQueryStatus::Success)
	{
		if (TArray<FVector> ResultLocations;QueryInstance->GetQueryResultsAsLocations(ResultLocations))
		{
			CachedOwnerComp->GetBlackboardComponent()->SetValueAsVector(HideLocationKey.SelectedKeyName, ResultLocations[0]);
			FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
		}
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
}
