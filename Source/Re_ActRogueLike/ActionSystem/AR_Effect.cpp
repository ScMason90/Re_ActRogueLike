// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect.h"

#include "AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Development/AR_DebugUtilities.h"

void UAR_Effect::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	if (Duration > 0.0f)
	{
		ResetDuration();
	}
	
	if (Period > 0.0f)
	{
		// Looped
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_Period, this, &ThisClass::ExecutePeriodicEffect, Period, true);
	}
}

void UAR_Effect::StopAction_Implementation()
{
	// Run 'PeriodEffect' once more if were about to 'tick' it at this frame.
	if (Period > 0.0f && GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle_Period) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodicEffect();
	}
	
	Super::StopAction_Implementation();
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Period);
	// Alternatively clear all timers explicitly (will also clear any timers added from Blueprint)
	// GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	GetOwningASComponent()->RemoveAction(this);
}

void UAR_Effect::ExecutePeriodicEffect_Implementation()
{
	// Do nothing here...
}

void UAR_Effect::ResetDuration()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, this, &ThisClass::StopAction, Duration);
}

void UAR_Effect::IncrementStackSize()
{
	StackCount++;
	
	if (bResetDurationOnStackIncrease)
	{
		// Refresh duration each increasing of stack. On expiration All stacks will be removed at once.  
		ResetDuration();
	}
	
	UE_LOG(LogGame, Log, TEXT("%s Incremented %s (%s) StackCount to %d"), *AR_DEBUG_LOC(), *GetName(), 
		*GetNameSafe(GetOwningASComponent()->GetOwner()), StackCount);
}
