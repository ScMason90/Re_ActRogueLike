// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

#include "AR_ActionSystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for ASComponent."), ECVF_Cheat);

// Sets default values for this component's properties
UAR_ActionSystemComponent::UAR_ActionSystemComponent()
{
	Attributes = FAR_AttributeSet();
}

void UAR_ActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	bWantsInitializeComponent = true;
}

bool UAR_ActionSystemComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	if (Delta < 0.0f)
	{
		if (!GetOwner()->CanBeDamaged()) return false;
		Delta *= CVarDamageMultiplier.GetValueOnGameThread();
	}
	
	float OldHealth = Attributes.Health;
	
	/* Simply clamp to validate Health variable.*/
	Attributes.Health = FMath::Clamp(Attributes.Health + Delta, 0, Attributes.MaxHealth);
	float ActualDelta = Attributes.Health - OldHealth;
	
	OnHealthChanged.Broadcast(Instigator, this, Attributes.Health, ActualDelta);
	
	// Died
	if (ActualDelta < 0.0f && GetHealth() == 0.0f)
	{
		AActor* Killer = nullptr;
		if (IsValid(Instigator) && !Instigator->IsPendingKillPending()) Killer = Instigator;
		if (IsValid(Instigator->GetInstigator())) Killer = Instigator->GetInstigator();
		
		AAR_GameModeBase* GM = GetWorld()->GetAuthGameMode<AAR_GameModeBase>();
		/*Passing 'Instigator->GetInstigator()' as Killer since 'Instigator' is quite like the 
		 *direct 'HitActor' throughout damage system... Or maybe we should improve our projectile class implementation?*/
		if (GM) GM->OnActorKilled(GetOwner(), Killer);
	}
	
	return ActualDelta != 0;
}

// return FMath::IsNearlyZero(Attributes.Health)...Restrictively check using return Attributes.Health == 0.0f; 
bool UAR_ActionSystemComponent::IsDead() const {return FMath::IsNearlyZero(GetHealth());}
float UAR_ActionSystemComponent::GetMaxHealth() const {return Attributes.MaxHealth;}
float UAR_ActionSystemComponent::GetHealth() const {return Attributes.Health;}
bool UAR_ActionSystemComponent::IsFullHealth() const {return GetHealth() == GetMaxHealth();}

UAR_ActionSystemComponent* UAR_ActionSystemComponent::GetASComp(AActor* FromActor)
{
	if (FromActor) return Cast<UAR_ActionSystemComponent>(FromActor->GetComponentByClass(StaticClass()));
	return nullptr;
}

bool UAR_ActionSystemComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}

void UAR_ActionSystemComponent::StartAction(FName InActionName)
{
	for (UAR_ActionSystem* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			Action->StartAction();
		}
	}
}
