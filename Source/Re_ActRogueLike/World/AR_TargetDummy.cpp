// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TargetDummy.h"

// Necessary compilation header files
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// Sets default values
AAR_TargetDummy::AAR_TargetDummy()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComponent;
	
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	// Do not assign to 'UAR_Pawn/Player/EnemyAttributeSet::StaticClass'...Any attribute set has potential UObject instances required.
	ActionSystemComponent->SetDefaultAttributeSet(UAR_HealthAttributeSet::StaticClass());
	
	// Trigger when health is changed (damage/healing)
	DelHandle_OnHealthChanged = UAR_GameplayStatics::BindOnMulticastAttrChangedLambda(
		ActionSystemComponent, SharedGameplayTags::Attribute_Health, [this](FGameplayTag Tag, float NewV, float OldV)
		{
			OnHealthChanged(Tag, NewV, OldV);
		});
	
}

float AAR_TargetDummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
								  AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// UE_LOG(LogGame, Log, TEXT("AAR_TargetDummy::TakeDamage, ActualDamage = %.4f"), ActualDamage);
	
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	
	return ActualDamage;
}

void AAR_TargetDummy::OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	if (bDummyDying)return;
	
	if (const bool bIsDead = UAR_GameplayStatics::IsDying(ActionSystemComponent))
	{
		bDummyDying = bIsDead;
		HandleDeath();
		return;
	}

	// 'HandleDamage()'?
	if (const float Delta = NewHealth - OldHealth; Delta < 0.0f)
	{
		MeshComponent->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

void AAR_TargetDummy::HandleDeath() const
{
	// Manually stop any possible processing 'Actions' or 'ActionEffects(Buff/Debuff)'
	for (const FGameplayTag& ActiveActionTag : ActionSystemComponent->GetActiveTags())
	{
		ActionSystemComponent->StopAction(ActiveActionTag);
	}
	
	// Remove multicast attribute delegate.
	UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(ActionSystemComponent, SharedGameplayTags::Attribute_Health, DelHandle_OnHealthChanged);
	
	// Disable Collision...Honestly all post-death appearances depend on your game type/design
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}
