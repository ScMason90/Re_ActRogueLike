// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeComponent.h"


// Sets default values for this component's properties
UAR_AttributeComponent::UAR_AttributeComponent()
{
	Health = MaxHealth = 100.0f;
	
}

bool UAR_AttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	
	/* Simply clamp to validate Health variable.*/
	Health = FMath::Clamp(Health, 0, MaxHealth);
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}

bool UAR_AttributeComponent::IsDead() const
{
	return Health <= 0.0f;
}
