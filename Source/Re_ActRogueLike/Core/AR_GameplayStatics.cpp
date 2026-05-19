// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameplayStatics.h"

#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"

bool UAR_GameplayStatics::IsFullHealth(UAR_ActionSystemComponent* ASComp)
{
	FAR_Attribute* Health = ASComp->GetAttribute(SharedGameplayTags::Attribute_Health);
	FAR_Attribute* HealthMax = ASComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax);
	
	return FMath::IsNearlyEqual(Health->GetValue(), HealthMax->GetValue());
}

bool UAR_GameplayStatics::IsDead(UAR_ActionSystemComponent* ASComp)
{
	FAR_Attribute* Health = ASComp->GetAttribute(SharedGameplayTags::Attribute_Health);
	
	return FMath::IsNearlyZero(Health->GetValue());
}
