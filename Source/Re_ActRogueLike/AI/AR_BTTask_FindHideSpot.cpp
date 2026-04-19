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
	
	// Start EQS Query (async)
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		AIController->GetWorld(), QueryTemplate,AIController->GetPawn(), 
		EEnvQueryRunMode::RandomBest5Pct, nullptr);
	
	if (!QueryInstance) return EBTNodeResult::Failed;
	// Save the OwnerComp pointer for callback use
	CachedOwnerComp = &OwnerComp;
	
	// Callback binding(ue5.7+ recommended)
	QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UAR_BTTask_FindHideSpot::OnQueryFinished);
	
	// BT asset wait for it's finished
	return EBTNodeResult::InProgress;
}

void UAR_BTTask_FindHideSpot::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	// Asynchronously check before use (assign in UAR_BTTask_FindHideSpot::ExecuteTask)
	if (!CachedOwnerComp) return;
	
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;	// This is for safely execute async 'FinishedTask'
	if (QueryStatus == EEnvQueryStatus::Success)
	{
		if (TArray<FVector> EnvQueryLocations;
			QueryInstance->GetQueryResultsAsLocations(EnvQueryLocations) && EnvQueryLocations.Num() > 0)
		{
			CachedOwnerComp->GetBlackboardComponent()->SetValueAsVector(
				HideLocationKey.SelectedKeyName, EnvQueryLocations[0]);
			NodeResult = EBTNodeResult::Succeeded;	// Can't finish(notify engine the result of this 'ExecuteTask') yet 
		}
	}
	
	// We only want exit(notify engine...) at the one the end of this function otherwise quirk happens.
	FinishLatentTask(*CachedOwnerComp, NodeResult);
}
