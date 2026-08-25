// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TeleportProjectile.h"

#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"

AAR_TeleportProjectile::AAR_TeleportProjectile()
{
	InitialLifeSpan = 0.0f;
	
	ProjectileMovementComponent->InitialSpeed = 6000.0f;
}

void AAR_TeleportProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
		TimerHandle_TeleportProj, this, &ThisClass::StartDelayedTeleport, DetonateDelay);
}

void AAR_TeleportProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	Super::EndPlay(EndPlayReason);
}

void AAR_TeleportProjectile::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Skip the base implementation, we handle our own as we must delay destroying and avoid playing duplicate explosion effects
	// Super::OnActorHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	// Cancel the timer to prevent a second teleportation
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	
	StartDelayedTeleport();
}

void AAR_TeleportProjectile::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Skip the base implementation, we handle our own as we must delay destroying and avoid playing duplicate explosion effects
	// Super::OnProjBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	// Cancel the timer to prevent a second teleportation
	GetWorldTimerManager().ClearTimer(TimerHandle_TeleportProj);
	
	StartDelayedTeleport();
}

void AAR_TeleportProjectile::StartDelayedTeleport()
{
	// The base class is responsible for VFX&SFX,Don't specify any CameraShake_BP inorder to keep clean teleport logic
	PlayExplosionFXs(FHitResult());

	// Prevent moving us further ahead while we wait for the teleport to trigger
	ProjectileMovementComponent->StopMovementImmediately();
	// Hide all visuals and prevent any further collision while we wait on the teleport timer
	LoopedNiagaraComponent->Deactivate();
	LoopedAudioComponent->Stop();
	SetActorEnableCollision(false);

	// Another delay until InstigatorActor teleports (so player has a chance to see the VFX before we move them)
	GetWorldTimerManager().SetTimer(
		TimerHandle_TeleportProj, this, &ThisClass::HandleTeleportation, TeleportDelay);
}

void AAR_TeleportProjectile::HandleTeleportation()
{
	// TODO: We should check if instigator is still valid/alive once we implement Death
	check(InstigatorActorRef);

	// Handles de-penetration on nearby collision geometry
	InstigatorActorRef->TeleportTo(GetActorLocation(), InstigatorActorRef->GetActorRotation());
	
	/* Note: the teleport call might fail if it cannot find any valid location,
	 * And when the player is shooting directly against the wall, due to the execution animation of the Cast action, the arm passes through the mold, causing the final transport bullet we shoot from the palm to wrongly "shoot through the wall". Before firing, perform a radiographic check from the generation position to determine the conditions or modify the Cast animation to avoid passing through the mold.
	 -> Need some algo to improve.Details need to be fine-tuned? */
	
	// UE_LOG(LogGame, Warning, TEXT("AAR_TeleportProjectile::HandleTeleportation();TeleProj location : %s"), *GetActorLocation().ToString());
	
	InstigatorActorRef->TeleportTo(GetActorLocation(), InstigatorActorRef->GetActorRotation());
	FVector PlayerLocation = InstigatorActorRef->GetActorLocation();
	
	// UE_LOG(LogGame, Error, TEXT("%s Player location : %s"), *AR_DEBUG_LOC(), *PlayerLocation.ToString());

	// Clear projectile from world, can't do this any sooner as that would prevent the timers from running on a valid Actor
	Destroy();
}
