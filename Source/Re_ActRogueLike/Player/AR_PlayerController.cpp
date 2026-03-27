// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

AAR_PlayerController::AAR_PlayerController()
{
	
}

void AAR_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}
