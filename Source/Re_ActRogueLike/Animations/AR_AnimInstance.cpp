// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AnimInstance.h"

#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

void UAR_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	ASComp = GetOwningActor()->FindComponentByClass<UAR_ActionSystemComponent>();
	
}

void UAR_AnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	ASComp->OnGameplayTagCountUpdated.AddDynamic(this, &ThisClass::OnTagUpdated);
	
}

void UAR_AnimInstance::OnTagUpdated(FGameplayTag UpdatedTag, int32 NewCount)
{
	bool bWasAdded = NewCount > 0;
	
	/* TODO : The lack of restrictions on players' movement and jumping operations leads to the Stunned animation 
	 * being wrongly blocked or interrupted, and the actual effect of Stunned needs to be enforced */ 
	if (UpdatedTag == SharedGameplayTags::StatusEffect_Sprinting)
	{
		bIsSprinting = bWasAdded;
	}
	else if (UpdatedTag == SharedGameplayTags::StatusEffect_Stunned)
	{
		bIsStunned = bWasAdded;
	}
	
}
