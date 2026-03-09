// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TeleportProjectile.h"

#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
AAR_TeleportProjectile::AAR_TeleportProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InitialLifeSpan = 0;
}

// Called when the game starts or when spawned
void AAR_TeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, 
		&AAR_TeleportProjectile::ExplodeAndTeleport, 0.2f);
}

void AAR_TeleportProjectile::ExplodeAndTeleport()
{
	Explode(nullptr);
	
	MovementComp->StopMovementImmediately();
	
	GetWorldTimerManager().SetTimer(TimerHandle_Teleport, 
		[this]()
		{
			if (APawn* Instigator = GetInstigator())
			{
				Instigator->TeleportTo(GetActorLocation(), Instigator->GetActorRotation());
			}
			
			Destroy();
		}, 0.2f, false);
}

// Called every frame
void AAR_TeleportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

