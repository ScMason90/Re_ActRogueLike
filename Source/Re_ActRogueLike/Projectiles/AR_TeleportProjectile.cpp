// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TeleportProjectile.h"

#include "TimerManager.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAR_TeleportProjectile::AAR_TeleportProjectile()
{
	InitialLifeSpan = 0.0f;
	
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
}

void AAR_TeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		TimerHandle_TeleportProj,
		this,
		&AAR_TeleportProjectile::ExplodeAndTeleport,
		DetonateDelay,
		false);
}

void AAR_TeleportProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	Super::EndPlay(EndPlayReason);
}

void AAR_TeleportProjectile::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Skip the base implementation, we handle our own as we must delay destroying and avoid playing duplicate explosion effects
	// Super::OnActorHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	// Cancel the timer to prevent a second teleportation
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	
	ExplodeAndTeleport();
}

void AAR_TeleportProjectile::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Skip the base implementation, we handle our own as we must delay destroying and avoid playing duplicate explosion effects
	// Super::OnProjBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	// Cancel the timer to prevent a second teleportation
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	
	ExplodeAndTeleport();
}

void AAR_TeleportProjectile::ExplodeAndTeleport()
{
	// The base class is responsible for VFX&SFX
	// Don't specify any CameraShake_BP inorder to keep clean teleport logic
	Explode(FHitResult());

	// Reinsurance
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->StopMovementImmediately();
	}

	FTimerDelegate TeleportDelegate;
	TeleportDelegate.BindWeakLambda(this, [this]()
	{
		if (!IsValid(this) || IsPendingKillPending())
		{
			return;
		}
		
		if (IsValid(InstigatorPawnActorRef))
		{
			UE_LOG(LogTemp, Warning, TEXT("AAR_TeleportProjectile::ExplodeAndTeleport();TeleProj location : %s"), *GetActorLocation().ToString());
			InstigatorPawnActorRef->TeleportTo(GetActorLocation(), InstigatorPawnActorRef->GetActorRotation());
			FVector PlayerLocation = InstigatorPawnActorRef->GetActorLocation();
			UE_LOG(LogTemp, Error, TEXT("AAR_TeleportProjectile::ExplodeAndTeleport();Player location : %s"), *PlayerLocation.ToString());
		}
		
		Destroy();
	});

	GetWorldTimerManager().SetTimer(
		TimerHandle_TeleportProj,
		TeleportDelegate,
		TeleportDelay,
		false);
}
