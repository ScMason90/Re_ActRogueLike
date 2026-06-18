// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect_Overwhelm.h"

#include "AR_ActionSystemComponent.h"
#include "AR_Effect_EnemyStunned.h"
#include "GameFramework/Actor.h"

UAR_Effect_Overwhelm::UAR_Effect_Overwhelm()
{
	// Stun
	OnStunThreshold = 3;
	StunnedEffectClass = UAR_Effect_EnemyStunned::StaticClass();
	
}

void UAR_Effect_Overwhelm::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	CurHit = 0;
	
	// Subscribe to injury incidents
	GetOwningASComponent()->GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamage);
}

void UAR_Effect_Overwhelm::OnTakeAnyDamage(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UAR_ActionSystemComponent* ASComp = GetOwningASComponent();
	CurHit++;
	
	// Of course, you can put in some effort to design a less crude Stun mechanism instead of this one LOL
	
	// Trigger Stun
	if (CurHit >= OnStunThreshold && StunnedEffectClass)
	{
		ASComp->GrantAction(StunnedEffectClass);
		CurHit = 0;	// Reset
	}
}

void UAR_Effect_Overwhelm::StopAction_Implementation()
{
	Super::StopAction_Implementation();
	
	// Remove subscription
	GetOwningASComponent()->GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::OnTakeAnyDamage);
}
