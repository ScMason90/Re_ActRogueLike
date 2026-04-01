// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PickupActorBase.h"

#include "TimerManager.h"
#include "Components/SphereComponent.h"


// Sets default values
AAR_PickupActorBase::AAR_PickupActorBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Pickup");
	RootComponent = SphereComp;
	
	RespawnTime = 5.0f;
}

void AAR_PickupActorBase::SetPickupState(bool bNewIsActivate)
{
	SetActorEnableCollision(bNewIsActivate);
	
	// Set visibility on root and all children
	RootComponent->SetVisibility(bNewIsActivate, true);
}

void AAR_PickupActorBase::ShowPickup()
{
	SetPickupState(true);
}

void AAR_PickupActorBase::HideAndCooldownPickup()
{
	SetPickupState(false);
	
	GetWorldTimerManager().SetTimer(
		HideAndCooldownPickup_TimerHandle, this, &AAR_PickupActorBase::ShowPickup, RespawnTime);
}

void AAR_PickupActorBase::Interact_Implementation(APawn* InstigatorPawn)
{
	// Logic in derived class...
	IAR_InteractionInterface::Interact_Implementation(InstigatorPawn);
	
}
