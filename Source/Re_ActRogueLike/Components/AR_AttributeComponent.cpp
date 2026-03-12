// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeComponent.h"


// Sets default values for this component's properties
UAR_AttributeComponent::UAR_AttributeComponent()
{
	Health = 100.0f;
	
}

bool UAR_AttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta;
	
	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);
	
	return true;
}
