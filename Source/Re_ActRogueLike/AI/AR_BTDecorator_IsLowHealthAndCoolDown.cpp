// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTDecorator_IsLowHealthAndCoolDown.h"

#include "AIController.h"
#include "Engine/World.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

UAR_BTDecorator_IsLowHealthAndCoolDown::UAR_BTDecorator_IsLowHealthAndCoolDown()
{
	NodeName = "LowHealth + Cooldown";
}

bool UAR_BTDecorator_IsLowHealthAndCoolDown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                                        uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;
	
	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return false;
	
	// Mainly for its Attributes variable
	UAR_ActionSystemComponent* ASComp = Pawn->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ASComp) return false;
	
	// Low Health Check - adjust threshold if changing design
	bool bLowHealth = ASComp->GetHealth() <= ASComp->GetMaxHealth() * LowHealthThreshold;
	if (!bLowHealth) return false;
	
	// Cooldown check - We can't precisely check equality of float number.
	float PresentTime = Pawn->GetWorld()->GetTimeSeconds();
	if (PresentTime - LastTriggerTime < CoolDownTime) return false;
	
	// Update Cooldown time
	const_cast<UAR_BTDecorator_IsLowHealthAndCoolDown*>(this)->LastTriggerTime = PresentTime;
	
	return Super::CalculateRawConditionValue(OwnerComp, NodeMemory);// true in Super::
}
