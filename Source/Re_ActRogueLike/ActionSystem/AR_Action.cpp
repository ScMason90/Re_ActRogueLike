// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Action.h"

#include "AR_ActionSystemComponent.h"
#include "Engine/World.h"
#include "Logging/StructuredLog.h"


UAR_ActionSystemComponent* UAR_Action::GetOwningASComponent() const
{
	return Cast<UAR_ActionSystemComponent>(GetOuter());
}

void UAR_Action::StartAction_Implementation()
{
	bIsRunning = true;
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_Action::StartAction_Implementation(), Started Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName.ToString()), ("WorldTime", GameTime));
}

void UAR_Action::StopAction_Implementation()
{
	bIsRunning = false;
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_Action::StopAction_Implementation(), Stopped Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName.ToString()), ("WorldTime", GameTime));
	
	CooldownThreshold = GameTime + CooldownTime;
}

bool UAR_Action::CanStart() const
{
	if (IsRunning()) return false;
	if (GetCooldownTimeRemaining() > 0.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("UAR_Action::CanStart(),Cooldown remaining: %f"), GetCooldownTimeRemaining());
		return false;
	}
	
	return true;
}

float UAR_Action::GetCooldownTimeRemaining() const
{
	return FMath::Max(0.0f, CooldownThreshold - GetWorld()->TimeSeconds);	
}
