// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Re_ActRogueLike/Player/AR_InteractionComponent.h"

AAR_PlayerController::AAR_PlayerController()
{
	// In derived data only BP class - ClassDefaults - Controller - Transform...
	// Set '...Attach to Pawn' as true so that the InteractionComponent will match the position of this Controller Class's PawnOwner 
	
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
