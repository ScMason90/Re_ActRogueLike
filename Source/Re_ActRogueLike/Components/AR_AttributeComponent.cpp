// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeComponent.h"


// Sets default values for this component's properties
UAR_AttributeComponent::UAR_AttributeComponent()
{
	Health = MaxHealth = 100.0f;
	
}

bool UAR_AttributeComponent::ApplyHealthChange(float Delta)
{
	float OldHealth = Health, ActualDelta;
	
	/* Simply clamp to validate Health variable.*/
	Health = FMath::Clamp(Health + Delta, 0, MaxHealth);
	ActualDelta = Health - OldHealth;
	
	// @fixme: Still nullptr for InstigatorActor parameter
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);
	
	return ActualDelta != 0;
}

bool UAR_AttributeComponent::IsDead() const {return Health <= 0.0f;}
float UAR_AttributeComponent::GetMaxHealth() const {return MaxHealth;}
bool UAR_AttributeComponent::IsFullHealth() const {return Health == MaxHealth;}
