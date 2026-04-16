// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"


// Sets default values for this component's properties
UAR_ActionSystemComponent::UAR_ActionSystemComponent()
{
	Attributes = FAR_AttributeSet();
}

bool UAR_ActionSystemComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	float OldHealth = Attributes.Health, ActualDelta;
	
	/* Simply clamp to validate Health variable.*/
	Attributes.Health = FMath::Clamp(Attributes.Health + Delta, 0, Attributes.MaxHealth);
	ActualDelta = Attributes.Health - OldHealth;
	
	OnHealthChanged.Broadcast(Instigator, this, Attributes.Health, ActualDelta);
	UE_LOG(LogTemp, Log, TEXT("New Health: %f, Max Health: %f"), Attributes.Health, Attributes.MaxHealth);
	
	return ActualDelta != 0;
}

// return FMath::IsNearlyZero(Attributes.Health)...Restrictively check using return Attributes.Health == 0.0f; 
bool UAR_ActionSystemComponent::IsDead() const {return FMath::IsNearlyZero(GetHealth());}
float UAR_ActionSystemComponent::GetMaxHealth() const {return Attributes.MaxHealth;}
float UAR_ActionSystemComponent::GetHealth() const {return Attributes.Health;}
bool UAR_ActionSystemComponent::IsFullHealth() const {return GetHealth() == GetMaxHealth();}
