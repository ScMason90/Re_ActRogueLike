// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PickupActorBase.h"

#include "TimerManager.h"
#include "Components/SphereComponent.h"


// Sets default values
AAR_PickupActorBase::AAR_PickupActorBase()
{
	OverlapComponent = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapComp"));
	// Always start with decent defaults, let Blueprint decide the final tweaked values
	OverlapComponent->SetSphereRadius(128.0f);
	RootComponent = OverlapComponent;
}

void AAR_PickupActorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &AAR_PickupActorBase::OnActorOverlapped);
}

void AAR_PickupActorBase::OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Do nothing here
}
