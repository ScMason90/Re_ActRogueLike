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
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_Action::StartAction_Implementation(), Started Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName), ("WorldTime", GameTime));
}

void UAR_Action::StopAction_Implementation()
{
	float GameTime = GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_Action::StopAction_Implementation(), Stopped Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName), ("WorldTime", GameTime));
}