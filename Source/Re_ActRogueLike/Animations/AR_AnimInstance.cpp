// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AnimInstance.h"

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
	
	if (UpdatedTag.MatchesTag(SharedGameplayTags::StatusEffect_Sprinting))
	{
		/* Now, if you start a Sprint immediately after 'FireProj', the ControlRig will look a bit awkward and 
		 * stiff overall because the FireMontage Cast animation has not yet ended and the transition from 
		 * the hover animation state used in the sprint is not smooth. Of course, you can hard-code the action duration 
		 * to calculate the end of the cast animation as a reference and prohibit the execution of any actions that 
		 * affect the animation performance during this period. However, a better approach should be to adjust and modify 
		 * the relevant animation BP assets.*/
		bIsSprinting = bWasAdded;
	}
	else if (UpdatedTag.MatchesTag(SharedGameplayTags::StatusEffect_Stunned))
	{
		bIsStunned = bWasAdded;
	}
	
}
