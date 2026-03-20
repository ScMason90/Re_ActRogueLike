// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeComponent.h"


// Sets default values for this component's properties
UAR_AttributeComponent::UAR_AttributeComponent()
{
	Health = 100.0f;
	
}

bool UAR_AttributeComponent::ApplyHealthChange(float Delta)
{
	/* TODO: No validating check and clamp([0,max]) solution either in cpp or bp yet.*/
	Health += Delta;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}

bool UAR_AttributeComponent::IsDead() const
{
	return Health < 0.0f;
}
