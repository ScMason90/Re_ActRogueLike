// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect_Overwhelm.h"

#include "AR_ActionSystemComponent.h"
#include "AR_Effect_EnemyStunned.h"
#include "GameFramework/Actor.h"

UAR_Effect_Overwhelm::UAR_Effect_Overwhelm()
{
	MaxOverwhelmPoints = 3;
	StunnedEffectClass = UAR_Effect_EnemyStunned::StaticClass();
}

void UAR_Effect_Overwhelm::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	CurrentOverwhelmPoints = 0;
	
	// Subscribe to injury incidents
	GetOwningASComponent()->GetOwner()->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamage);
}

void UAR_Effect_Overwhelm::OnTakeAnyDamage(AActor* DamagedActor, float Damage,
	const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentOverwhelmPoints++;
	
	// Of course you can put in some effort to design a less crude Stun mechanism instead of this one LOL
	if (CurrentOverwhelmPoints >= MaxOverwhelmPoints)
	{
		// Trigger Stun
		GetOwningASComponent()->GrantAction(StunnedEffectClass);
		
		// Reset
		CurrentOverwhelmPoints = 0;
	}
}

void UAR_Effect_Overwhelm::StopAction_Implementation()
{
	Super::StopAction_Implementation();
	
	// Remove subscription
	GetOwningASComponent()->GetOwner()->OnTakeAnyDamage.RemoveDynamic(this, &ThisClass::OnTakeAnyDamage);
}
