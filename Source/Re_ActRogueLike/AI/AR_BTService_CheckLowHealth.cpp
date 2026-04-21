// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTService_CheckLowHealth.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

UAR_BTService_CheckLowHealth::UAR_BTService_CheckLowHealth()
{
	NodeName = "Check Low Health (with Cooldown) Service";
	Interval = 0.5f; // check every 0.5s
}

void UAR_BTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return;

	UAR_ActionSystemComponent* ASComp/*Mainly for its 'Attribute'*/ = Pawn->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ASComp) return;

	bool bLowHealth = ASComp->GetHealth() <= ASComp->GetMaxHealth() * LowHealthThreshold;

	float Now = Pawn->GetWorld()->GetTimeSeconds();
	bool bCooldownReady = (Now - LastTriggerTime) > CooldownTime;

	if (bLowHealth && bCooldownReady)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ShouldFleeKey.SelectedKeyName, true);
		LastTriggerTime = Now;
	}
	else OwnerComp.GetBlackboardComponent()->SetValueAsBool(ShouldFleeKey.SelectedKeyName, false);
}
