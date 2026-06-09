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

void UAR_AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ASComp)
	{
		// TODO: Update to use delegates from action system
		bIsSprinting = ASComp->ActiveGameplayTags.HasTag(SharedGameplayTags::StatusEffect_Sprinting);
		bIsStunned = ASComp->ActiveGameplayTags.HasTag(SharedGameplayTags::StatusEffect_Stunned);
	}

}
