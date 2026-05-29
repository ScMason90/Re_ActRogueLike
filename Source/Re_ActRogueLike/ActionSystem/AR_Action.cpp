// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Action.h"

#include "AR_ActionSystemComponent.h"
#include "Engine/World.h"
#include "Logging/StructuredLog.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"


UAR_ActionSystemComponent* UAR_Action::GetOwningASComponent() const
{
	return Cast<UAR_ActionSystemComponent>(GetOuter());
}

void UAR_Action::StartAction_Implementation()
{
	bIsRunning = true;
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogGame, Log, "UAR_Action::StartAction_Implementation(), Started Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName.ToString()), ("WorldTime", GameTime));
	
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	OwningComp->ActiveGameplayTags.AppendTags(GrantTags);
	
	// Consume required resources
	for (TPair<FGameplayTag, float> Cost : ActivationCost)
	{
		OwningComp->ApplyAttributeChanged(Cost.Key, -Cost.Value, Modifier);
	}
}

void UAR_Action::StopAction_Implementation()
{
	bIsRunning = false;
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogGame, Log, "UAR_Action::StopAction_Implementation(), Stopped Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName.ToString()), ("WorldTime", GameTime));
	
	CooldownThreshold = GameTime + CooldownTime;
	
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	OwningComp->ActiveGameplayTags.RemoveTags(GrantTags);
	
}

bool UAR_Action::CanStart() const
{
	if (IsRunning()) return false;
	if (GetCooldownTimeRemaining() > 0.0f)
	{
		UE_LOG(LogGame, Log, TEXT("UAR_Action::CanStart(),Cooldown remaining: %f"), GetCooldownTimeRemaining());
		return false;
	}
	
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	if (OwningComp->ActiveGameplayTags.HasAny(BlockedTags)/*hierarchy involved*/) return false;
	
	for (TPair<FGameplayTag, float> Cost : ActivationCost)
	{
		float AvailableAttributeAmount = OwningComp->GetAttributeValue(Cost.Key);
		if (AvailableAttributeAmount < Cost.Value)
		{
			// Not enough resources
			UE_LOGFMT(LogGame, Log, "UAR_Action::CanStart(), Not enough {AttributeName} to activate {Action}, "
						   "Have {AvailableAttributeAmount} and need {RequiredAttributeValue}",
						   ("AttributeName", Cost.Key.ToString()), ("Action", ActionName.ToString()),
						   ("AvailableAttributeAmount", AvailableAttributeAmount), ("RequiredAttributeValue", Cost.Value));
			
			return false;
		}
	}
	
	return true;
}

float UAR_Action::GetCooldownTimeRemaining() const
{
	return FMath::Max(0.0f, CooldownThreshold - GetWorld()->TimeSeconds);	
}
