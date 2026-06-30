// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BTDecorator_IsLowHealthAndCoolDown.h"

#include "AIController.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
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
	
	UAR_ActionSystemComponent* ASComp = AICon->GetPawn()->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!ASComp) return false;
	ensure(ASComp);
	
	// Low Health Check - adjust threshold if changing design
	float CurHealth = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Health),
		  MaxHealth = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_HealthMax);
	bool bLowHealth = CurHealth <= MaxHealth * LowHealthThreshold;
	if (!bLowHealth) return false;
	
	// Cooldown check - We can't precisely check equality of float number.
	FBTDecoratorLowHealthMemory* MyMemory = reinterpret_cast<FBTDecoratorLowHealthMemory*>(NodeMemory);
	float PresentTime = ASComp->GetWorld()->GetTimeSeconds();
	if (PresentTime - MyMemory->LastTriggerTime < CoolDownTime) return false;
	
	// Update Cooldown time
	MyMemory->LastTriggerTime = PresentTime;
	
	return true;// Super::CalculateRawConditionValue(OwnerComp, NodeMemory)
}

uint16 UAR_BTDecorator_IsLowHealthAndCoolDown::GetInstanceMemorySize() const
{
	return sizeof(FBTDecoratorLowHealthMemory);
}
