// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect_Overwhelm.h"

#include "AR_ActionSystemComponent.h"
#include "AR_Effect_EnemyStunned.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"

UAR_Effect_Overwhelm::UAR_Effect_Overwhelm()
{
	// Since 'EnemyStun' is entirely building in C++, We can directly assign it here instead of referencing Blueprint class
	EffectOnThreshold = UAR_Effect_EnemyStunned::StaticClass();
	// Note:Do not derive any BP class from this Overwhelm debuff
	
}

void UAR_Effect_Overwhelm::OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth)
{
	const float DeltaHealth = NewHealth - OldHealth;
	if (DeltaHealth >= 0.0f) return;	// Ignore 'invalid damage' and healing
	
	// Don't accumulate damage until debuff has expired (avoid debuff-locking)
	if (GetOwningASComponent()->GetActiveTags().HasTag(SharedGameplayTags::StatusEffect_Stunned))
	{
		return;	// Note that right now at 'ThisClass' it's a hard-coded locking check for 'Stun' debuff.
		/* If a BP ActionEffect asset created based on the cpp class is specified as the EffectOnThreshold 
		 * using TObjectPtr, then its GrantTags can be accessed for lock checks. But that means we also need a blueprint
		 * asset created based on ThisClass. This should be contrary to Tomlooman's requirements for assignment6.*/
	}
	
	const float TimeNow = GetWorld()->TimeSeconds;
	const float DeltaTime = TimeNow - LastDamageTime;
	
	// Did less time pass than the threshold?
	if (DeltaTime < TimeDeltaThreshold) SummedRecentDamage += FMath::Abs(DeltaHealth);
	else SummedRecentDamage = FMath::Abs(DeltaHealth);	// Outside 'TimeDeltaThreshold', Reset to only current damage.
	
	LastDamageTime = TimeNow;
	
	// Trigger stun?
	if (SummedRecentDamage >= DamageThreshold)
	{
		GetOwningASComponent()->GrantAction(EffectOnThreshold);
		
		// Trigger log in UAR_Action::StartAction_Implementation() - UE_LOG...
		
		// Reset now that the stun is applied
		SummedRecentDamage = 0.0f;
		LastDamageTime = 0.0f;
	}
}

void UAR_Effect_Overwhelm::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	DelegateHandle_Health = GetOwningASComponent()->GetAttributeListener(
		SharedGameplayTags::Attribute_Health).AddUObject(this, &ThisClass::OnHealthChanged);
}

void UAR_Effect_Overwhelm::StopAction_Implementation()
{
	Super::StopAction_Implementation();
	
	// Clear as soon as we expired/removed/stop, Otherwise we continue receiving delegates from 'OnHealthChanged' 
	GetOwningASComponent()->GetAttributeListener(SharedGameplayTags::Attribute_Health).Remove(DelegateHandle_Health);
}
