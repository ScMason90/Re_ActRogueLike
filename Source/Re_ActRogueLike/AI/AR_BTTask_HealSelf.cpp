// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTTask_HealSelf.h"

#include "AIController.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

UAR_BTTask_HealSelf::UAR_BTTask_HealSelf()
{
	NodeName = "HealSelf";	
}

EBTNodeResult::Type UAR_BTTask_HealSelf::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	check(Pawn);
	
	UAR_ActionSystemComponent* ASComp = Pawn->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ensure(ASComp)) return EBTNodeResult::Failed;
	
	ASComp->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, HealAmount, Base);
	
	return EBTNodeResult::Succeeded;	// Super::ExecuteTask(OwnerComp, NodeMemory) 
}
