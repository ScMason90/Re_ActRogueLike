#include "AR_TeleportProjectile.h"

#include "TimerManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAR_TeleportProjectile::AAR_TeleportProjectile()
{
	InitialLifeSpan = 0.0f;
	
	MovementComp->InitialSpeed = 1300.0f;
}

void AAR_TeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		TimerHandle_Explode,
		this,
		&AAR_TeleportProjectile::ExplodeAndTeleport,
		0.2f,
		false);
}

void AAR_TeleportProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Explode);
	GetWorldTimerManager().ClearTimer(TimerHandle_Teleport);

	Super::EndPlay(EndPlayReason);
}

void AAR_TeleportProjectile::ExplodeAndTeleport()
{
	if (IsPendingKillPending())
	{
		return;
	}

	// The base class is responsible for vision and stopping movement
	Explode(FHitResult());

	// Reinsurance once
	if (MovementComp)
	{
		MovementComp->StopMovementImmediately();
	}

	FTimerDelegate TeleportDelegate;
	TeleportDelegate.BindWeakLambda(this, [this]()
	{
		if (!IsValid(this) || IsPendingKillPending())
		{
			return;
		}
		
		if (APawn* InstigatorPawn = GetInstigator())
		{
			UE_LOG(LogTemp, Warning, TEXT("AR_TeleProj location : %s"), *GetActorLocation().ToString());
			InstigatorPawn->TeleportTo(GetActorLocation(), InstigatorPawn->GetActorRotation());
			FVector PlayerLocation = InstigatorPawn->GetActorLocation();
			UE_LOG(LogTemp, Error, TEXT("Player location : %s"), *PlayerLocation.ToString());
		}
		
		Destroy();
	});

	GetWorldTimerManager().SetTimer(
		TimerHandle_Teleport,
		TeleportDelegate,
		0.2f,
		false);
}
