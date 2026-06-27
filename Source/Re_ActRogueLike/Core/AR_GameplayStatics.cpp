// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameplayStatics.h"

#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"

// You can replace all 'ASComp->GetAttribute()' to 'ASComp->GetAttributeValue()' for convenience.

bool UAR_GameplayStatics::IsFullHealth(UAR_ActionSystemComponent* ASComp)
{
	check(ASComp);
	
	FAR_Attribute* Health = ASComp->GetAttribute(SharedGameplayTags::Attribute_Health);
	FAR_Attribute* HealthMax = ASComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax);
	
	return FMath::IsNearlyEqual(Health->GetValue(), HealthMax->GetValue());
}

bool UAR_GameplayStatics::IsDying(UAR_ActionSystemComponent* ASComp)
{
	check(ASComp);
	
	float HealthValue = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Health);
	
	return FMath::IsNearlyZero(HealthValue) /*|| HealthValue < 0.0f*/;
}

bool UAR_GameplayStatics::IsActorAlive(AActor* ActorToCheck)
{
	if (ActorToCheck)
	{
		if (UAR_ActionSystemComponent* ASComp = ActorToCheck->FindComponentByClass<UAR_ActionSystemComponent>())
		{
			return ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Health) > 0.0f;
		}
	}
	
	return false;
}
