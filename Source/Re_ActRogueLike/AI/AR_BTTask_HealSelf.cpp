// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTTask_HealSelf.h"

#include "AIController.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

UAR_BTTask_HealSelf::UAR_BTTask_HealSelf()
{
	NodeName = "HealSelf";	
}

EBTNodeResult::Type UAR_BTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;
	
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;
	
	UAR_ActionSystemComponent* ASComp/*Mainly for its 'Attribute'*/ = Pawn->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ASComp) return EBTNodeResult::Failed;
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);	// EBTNodeResult::Succeeded
}
