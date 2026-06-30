// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ExplosiveBarrel.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AAR_ExplosiveBarrel::AAR_ExplosiveBarrel()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetSimulatePhysics(true);
	// Blueprint automatically sets this collision profile when enabling simulate physics, in C++ we do this manually.
	MeshComponent->SetCollisionProfileName("PhysicsActor");
	RootComponent = MeshComponent;
	
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	// No need to 'AutoActivate' this, that is only good for applying a constant radial force and not our desired one frame Impulse
	RadialForceComponent->SetAutoActivate(false);
	
	// Must be quite large, depends on the Mass of the Actors we intend to launch such as the Cubes
	RadialForceComponent->ImpulseStrength = 1500.0f;	// Alternative: 2000000.0f if bImpulseVelChange is false.
	RadialForceComponent->Radius = 750.0f;
	// Optional, ignores 'Mass' of other objects 
	RadialForceComponent->bImpulseVelChange = true;	//(if false, the impulse strength will be much higher to push most objects depending on Mass)
	
}

void AAR_ExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Register hit event so the barrel reacts to physics collisions ->  Please note not to repeat with 'TakeDamage'
	// MeshComp->OnComponentHit.AddDynamic(this, &AAR_ExplosiveBarrel::OnActorHit);
	
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);	// Ensure the barrel is treated as a physics body
	
	// Optional, default constructor of component already has 4 object types to affect, exclude WorldDynamic.
	RadialForceComponent->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// TODO: Note that BP class instance derived from this cpp class can only 'explode' once
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
	
	// Crashed here if 'Active...VFX/SFXComp' is nullptr due to slow loading. Considering introduce some Async delegate callback?
	if (ActiveBurningVFXComp)ActiveBurningVFXComp->Deactivate();
	if (ActiveBurningSFXComp)ActiveBurningSFXComp->Stop();
	
	RadialForceComponent->FireImpulse();
	
	MeshComponent->AddImpulse(FVector::UpVector * 1000, NAME_None, true);
	MeshComponent->AddAngularImpulseInDegrees(FVector::RightVector * 1000, NAME_None, true);
	
	FVector BarrelLoc = GetActorLocation();
	FRotator BarrelRot = GetActorRotation();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionVFX, BarrelLoc, BarrelRot);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, BarrelLoc);
}