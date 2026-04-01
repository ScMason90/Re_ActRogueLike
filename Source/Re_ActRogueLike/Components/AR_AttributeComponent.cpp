// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeComponent.h"


// Sets default values for this component's properties
UAR_AttributeComponent::UAR_AttributeComponent()
{
	Attributes = FAR_AttributeSet();
}

bool UAR_AttributeComponent::ApplyHealthChange(AActor* Instigator, float Delta)
{
	float OldHealth = Attributes.Health, ActualDelta;
	
	/* Simply clamp to validate Health variable.*/
	Attributes.Health = FMath::Clamp(Attributes.Health + Delta, 0, Attributes.MaxHealth);
	ActualDelta = Attributes.Health - OldHealth;
	
	OnHealthChanged.Broadcast(Instigator, this, Attributes.Health, ActualDelta);
	UE_LOG(LogTemp, Log, TEXT("New Health: %f, Max Health: %f"), Attributes.Health, Attributes.MaxHealth);
	
	return ActualDelta != 0;
}

bool UAR_AttributeComponent::IsDead() const {return Attributes.Health <= 0.0f;}
float UAR_AttributeComponent::GetMaxHealth() const {return Attributes.MaxHealth;}
float UAR_AttributeComponent::GetHealth() const {return Attributes.Health;}
bool UAR_AttributeComponent::IsFullHealth() const {return Attributes.Health == Attributes.MaxHealth;}
