// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Effect.h"

#include "AR_ActionSystemComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UAR_Effect::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	if (Duration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_Duration, this, &ThisClass::StopAction, Duration);
	}
}

void UAR_Effect::StopAction_Implementation()
{
	Super::StopAction_Implementation();
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Duration);
	
	GetOwningASComponent()->RemoveAction(this);
}
