// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Action.h"

#include "AR_ActionSystemComponent.h"
#include "Logging/StructuredLog.h"


UAR_ActionSystemComponent* UAR_Action::GetOwningASComponent() const
{
	return Cast<UAR_ActionSystemComponent>(GetOuter());
}

void UAR_Action::StartAction()
{
	float GameTime = 0.0f;	// GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_Action::StartAction(), Started Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName), ("WorldTime", GameTime));
}