// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystem.h"

#include "Logging/StructuredLog.h"


void UAR_ActionSystem::StartAction()
{
	float GameTime = 0.0f;	// GetWorld()->TimeSeconds;
	
	UE_LOGFMT(LogTemp, Log, "UAR_ActionSystem::StartAction(), Started Action {ActionName} - {WorldTime}", 
		("ActionName", ActionName), ("WorldTime", GameTime));
}
