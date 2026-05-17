// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

#include "AR_Action.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for ASComponent."), ECVF_Cheat);

// Sets default values for this component's properties
UAR_ActionSystemComponent::UAR_ActionSystemComponent()
{
	/*bWantsInitializeComponent should be set in class object constructor not in virtual InitializeComponent() override 
	which should be its consequence call*/ 
	bWantsInitializeComponent = true;
	
	Attributes = FAR_AttributeSet();
}

void UAR_ActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	// UE_LOGFMT(LogTemp, Error, 
	// 	"UAR_ActionSystemComponent::InitializeComponent(), Is DefaultActions empty?{Answer}", DefaultActions.IsEmpty()?"Yes":"No");
	for (TSubclassOf<UAR_Action> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass)) GrantAction(ActionClass);
	}
}

void UAR_ActionSystemComponent::GrantAction(TSubclassOf<UAR_Action> NewActionClass)
{
	UAR_Action* NewAction = NewObject<UAR_Action>(this, NewActionClass);
	Actions.Add(NewAction);
}

void UAR_ActionSystemComponent::StartAction(FGameplayTag InActionName)
{
	// UE_LOGFMT(LogCore, Warning, 
	// 	"UAR_ActionSystemComponent::StartAction,Is Actions empty?{Answer}", Actions.IsEmpty()?"Yes":"No");
	
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			if (Action->CanStart()) Action->StartAction();
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, 
		TEXT("UAR_ActionSystemComponent::StartAction,No Action found with name %s"), *InActionName.ToString());
}

void UAR_ActionSystemComponent::StopAction(FGameplayTag InActionName)
{
	// UE_LOGFMT(LogCore, Warning, 
	// 	"UAR_ActionSystemComponent::StopAction,Is Actions empty?{Answer}", Actions.IsEmpty()?"Yes":"No");
	
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			Action->StopAction();
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, 
		TEXT("UAR_ActionSystemComponent::StopAction,No Action found with name %s"), *InActionName.ToString());
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
bool UAR_ActionSystemComponent::IsDead() const
{
	return FMath::IsNearlyZero(Attributes.Health);
}

float UAR_ActionSystemComponent::GetMaxHealth() const
{
	return Attributes.MaxHealth;
}

float UAR_ActionSystemComponent::GetHealth() const
{
	return Attributes.Health;
}

bool UAR_ActionSystemComponent::IsFullHealth() const
{
	return FMath::IsNearlyEqual(Attributes.Health, Attributes.MaxHealth);
}

UAR_ActionSystemComponent* UAR_ActionSystemComponent::GetASComp(AActor* FromActor)
{
	if (FromActor) return Cast<UAR_ActionSystemComponent>(FromActor->GetComponentByClass(StaticClass()));
	return nullptr;
}

bool UAR_ActionSystemComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetMaxHealth());
}
