// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect_EnemyStunned.h"

#include "Re_ActRogueLike/SharedGameplayTags.h"

UAR_Effect_EnemyStunned::UAR_Effect_EnemyStunned()
{
	// Undefined variable - ActionName
	
	// Depends on selected/using BP animation duration for 'RangedMinion'
	Duration = 2.5f;
	GrantTags.AddTag(SharedGameplayTags::StatusEffect_Stunned);
	
}
