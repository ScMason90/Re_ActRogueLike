// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Re_ActRogueLike/Components/AR_InteractionComponent.h"

AAR_PlayerController::AAR_PlayerController()
{
	InteractionComponent = CreateDefaultSubobject<UAR_InteractionComponent>(TEXT("InteractionComp"));
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

void AAR_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Triggered, this, &AAR_PlayerController::StartInteract);
}

void AAR_PlayerController::StartInteract()
{
	InteractionComponent->PrimaryInteraction();
}
