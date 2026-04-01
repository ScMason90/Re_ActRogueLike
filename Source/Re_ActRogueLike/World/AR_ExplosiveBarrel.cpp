// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ExplosiveBarrel.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
AAR_ExplosiveBarrel::AAR_ExplosiveBarrel()
{
	// Mesh with physics simulation
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetSimulatePhysics(true);
	// Blueprint automatically sets this collision profile when enabling simulate physics, in C++ we do this manually.
	MeshComponent->SetCollisionProfileName("PhysicsActor");
	RootComponent = MeshComponent;
	
	// Radial force for explosion behavior
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	// We don't need to Activate this component, that is only good for applying a constant radial force and not our desired one frame Impulse
	RadialForceComponent->SetAutoActivate(false);
	
	// Must be quite large, depends on the Mass of the Actors we intend to launch such as the Cubes
	RadialForceComponent->ImpulseStrength = 1500.0f;	// Alternative: 2000000.0f if bImpulseVelChange is false.
	RadialForceComponent->Radius = 750.0f;
	// Optional, ignores 'Mass' of other objects 
	RadialForceComponent->bImpulseVelChange = true;	//(if false, the impulse strength will be much higher to push most objects depending on Mass)
	
	// Optional, default constructor of component already has 4 object types to affect, exclude WorldDynamic.
	RadialForceComponent->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

void AAR_ExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Register hit event so the barrel reacts to physics collisions
	// MeshComp->OnComponentHit.AddDynamic(this, &AAR_ExplosiveBarrel::OnActorHit);
	
	// Ensure the barrel is treated as a physics body
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
}

// TODO: For somehow, bp instance derived from current cpp class set can only 'explode' once
float AAR_ExplosiveBarrel::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Allow exploding once, also skip when timer is already running
	if (bExploded || GetWorldTimerManager().TimerExists(ExplosionTimerHandle))
	{
		return ActualDamage;
	}
	
	// Hold onto both the Niagara and Audio Components to disable them during the Explode()
	ActiveBurningVFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(BurningVFX, MeshComponent, NAME_None,
		FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget,true);
	ActiveBurningSFXComp = UGameplayStatics::SpawnSoundAttached(BurningSFX, MeshComponent);
	
	GetWorldTimerManager().SetTimer(
		ExplosionTimerHandle, this,  &AAR_ExplosiveBarrel::Explode, ExplosionDelay);
	
	return ActualDamage;
}

void AAR_ExplosiveBarrel::Explode()
{
	bExploded = true;
	
	ActiveBurningVFXComp->Deactivate();
	ActiveBurningSFXComp->Stop();
	
	RadialForceComponent->FireImpulse();
	
	FVector BarrelLoc = GetActorLocation();
	FRotator BarrelRot = GetActorRotation();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		this, ExplosionVFX, BarrelLoc, BarrelRot);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, BarrelLoc);
}