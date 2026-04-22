// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTService_CheckLowHealth.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

UAR_BTService_CheckLowHealth::UAR_BTService_CheckLowHealth()
{
	NodeName = "BTService:Check Low Health (with Cooldown)";
	Interval = 0.5f; // check every 0.5s
}

void UAR_BTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return;

	UAR_ActionSystemComponent* ASComp/*Mainly for its 'Attribute'*/ = Pawn->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ASComp) return;

	bool bLowHealth = ASComp->GetHealth() <= ASComp->GetMaxHealth() * LowHealthThreshold;

	FBTServiceLowHealthMemory* MyMemory = reinterpret_cast<FBTServiceLowHealthMemory*>(NodeMemory);
	float Now = Pawn->GetWorld()->GetTimeSeconds();
	bool bCooldownReady = (Now - MyMemory->LastTriggerTime) > CooldownTime;

	UE_LOG(LogTemp, Warning, TEXT("UAR_BTService_CheckLowHealth::TickNode, bLowHealth = %hhd"), bLowHealth);
	UE_LOG(LogTemp, Warning, TEXT("UAR_BTService_CheckLowHealth::TickNode, bCooldownReady = %hhd"), bCooldownReady);
	if (bLowHealth && bCooldownReady)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ShouldFleeKey.SelectedKeyName, true);
		MyMemory->LastTriggerTime = Now;
	}
	else OwnerComp.GetBlackboardComponent()->SetValueAsBool(ShouldFleeKey.SelectedKeyName, false);
}

uint16 UAR_BTService_CheckLowHealth::GetInstanceMemorySize() const { return sizeof(FBTServiceLowHealthMemory); }
