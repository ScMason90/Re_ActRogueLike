// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeSet.h"

#include "AR_ActionSystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// class RE_ACTROGUELIKE_API UAR_AttributeSet : public UObject
// -----------------------------------------------------------

UAR_ActionSystemComponent* UAR_AttributeSet::GetOwningASComponent() const
{
	return Cast<UAR_ActionSystemComponent>(GetOuter());
}

// class UAR_HealthAttributeSet : public UAR_AttributeSet
// ------------------------------------------------------

UAR_HealthAttributeSet::UAR_HealthAttributeSet()
{
	Health = FAR_Attribute(100.0f);
	HealthMax = FAR_Attribute(Health.GetValue());
}

void UAR_HealthAttributeSet::PostAttributeChanged()
{
	// UE_LOG(LogGame, Warning, TEXT("UAR_HealthAttributeSet::PostAttributeChanged(), SomeWhere Health:%.2f before post changed"), Health.Base);
	
	Health.Base = FMath::Clamp(Health.Base, 0.0f, HealthMax.GetValue());
	
	// UE_LOG(LogGame, Warning, TEXT("UAR_HealthAttributeSet::PostAttributeChanged(), SomeWhere Health:%.2f after post changed"), Health.Base);
}

// class UAR_PawnAttributeSet : public UAR_HealthAttributeSet
// ----------------------------------------------------------

UAR_PawnAttributeSet::UAR_PawnAttributeSet()
{
	MoveSpeed = FAR_Attribute(550);
	MoveSpeedMultiplier = FAR_Attribute(1.0f);
	
}

void UAR_PawnAttributeSet::InitializeAttributes()
{
	Super::InitializeAttributes();
	
	ApplyMoveSpeed();
}

void UAR_PawnAttributeSet::PostAttributeChanged()
{
	Super::PostAttributeChanged();
	
	ApplyMoveSpeed();
}

void UAR_PawnAttributeSet::ApplyMoveSpeed()
{
	ACharacter* Character = Cast<ACharacter>(GetOwningASComponent()->GetOwner());
	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed.GetValue() * MoveSpeedMultiplier.GetValue();
}

// class UAR_PlayerAttributeSet : public UAR_PawnAttributeSet
// ----------------------------------------------------------

UAR_PlayerAttributeSet::UAR_PlayerAttributeSet()
{
	
}

// class UAR_EnemyAttributeSet : public UAR_PawnAttributeSet
// ---------------------------------------------------------

UAR_EnemyAttributeSet::UAR_EnemyAttributeSet()
{
	MoveSpeed = FAR_Attribute(450);
	
}