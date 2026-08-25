// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Action.h"

// Necessary compilation header files
#include "AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Re_ActLogChannels.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



UAR_ActionSystemComponent* UAR_Action::GetOwningASComponent() const
{
	return Cast<UAR_ActionSystemComponent>(GetOuter());
}

FString UAR_Action::GetGrantTagsAsString() const
{
#if !UE_BUILD_SHIPPING
	return GrantTags.ToString();
#else
	return TEXT("");	// Nothing when Shipping as playable game  
#endif
}

void UAR_Action::StartAction_Implementation()
{
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	OwningComp->AppendActiveTags(GrantTags);
	
	// Consume required resources
	for (TPair<FGameplayTag, float> Cost : ActivationCost)
	{
		OwningComp->ApplyAttributeChanged(Cost.Key, -Cost.Value, Modifier);
	}
	
	bIsRunning = true;
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogGame, Log, 
		"{LogLoc}, {OwnerActor}, Started {ActionName} - {WorldTime}", ("LogLoc", AR_LOG_LOC()),
		("ActionName", ActionName.IsValid() ? ActionName.ToString()/*Action*/ : GrantTags.ToString()/*Effect*/), 
		("WorldTime", GameTime), ("OwnerActor", GetNameSafe(OwningComp->GetOwner())));
}

void UAR_Action::StopAction_Implementation()
{
	bIsRunning = false;
	float GameTime = GetWorld()->TimeSeconds;
	
	CooldownThreshold = GameTime + CooldownTime;
	
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	OwningComp->RemoveActiveTags(GrantTags);
	
	UE_LOGFMT(LogGame, Log, 
		"{LogLoc}, {OwnerActor}, Started {ActionName} - {WorldTime}", ("LogLoc", AR_LOG_LOC()),
		("ActionName", ActionName.IsValid() ? ActionName.ToString()/*Action*/ : GrantTags.ToString()/*Effect*/), 
		("WorldTime", GameTime), ("OwnerActor", GetNameSafe(OwningComp->GetOwner())));
	
}

bool UAR_Action::CanStart() const
{
	if (IsRunning()) return false;
	if (GetCooldownTimeRemaining() > 0.0f)
	{
		UE_LOG(LogGame, Log, TEXT("%s Cooldown remaining: %f"), *AR_LOG_LOC(), GetCooldownTimeRemaining());
		return false;
	}
	
	UAR_ActionSystemComponent* OwningComp = GetOwningASComponent();
	if (OwningComp->GetActiveTags().HasAny(BlockedTags/*hierarchy involved*/)) return false;
	
	for (TPair<FGameplayTag, float> Cost : ActivationCost)
	{
		float AvailableAttributeAmount = OwningComp->GetAttributeValue(Cost.Key);
		if (AvailableAttributeAmount < Cost.Value)
		{
			// Not enough resources
			UE_LOGFMT(LogGame, Log, "{LogLoc}, Not enough {AttributeName} to activate {Action}, "
						   "Have {AvailableAttributeAmount} and need {RequiredAttributeValue}", ("LogLoc", AR_LOG_LOC()), 
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
