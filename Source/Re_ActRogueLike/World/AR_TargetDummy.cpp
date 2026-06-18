// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TargetDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"


// Sets default values
AAR_TargetDummy::AAR_TargetDummy()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComponent;
	
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	// Do not assign to 'UAR_Pawn/Player/EnemyAttributeSet::StaticClass'...Any attribute set has potential UObject instances required.
	ActionSystemComponent->SetDefaultAttributeSet(UAR_HealthAttributeSet::StaticClass());
	
	// Trigger when health is changed (damage/healing)
	FOnAttributeChanged& Event = ActionSystemComponent->GetAttributeListener(SharedGameplayTags::Attribute_Health);
	Event.AddUObject(this, &ThisClass::OnHealthChanged);
	
}

void AAR_TargetDummy::OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	if (bDummyDying)return;
	
	if (const bool bIsDead = UAR_GameplayStatics::IsDying(ActionSystemComponent))
	{
		bDummyDying = bIsDead;
		// 'HandleDeath()' for test something?
		return;
	}

	// 'HandleDamage()'?
	if (const float Delta = NewHealth - OldHealth; Delta < 0.0f)
	{
		MeshComponent->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

float AAR_TargetDummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	
	return ActualDamage;
}
