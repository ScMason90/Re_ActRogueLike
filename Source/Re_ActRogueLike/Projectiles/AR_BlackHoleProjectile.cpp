// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_BlackHoleProjectile.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


// Sets default values
AAR_BlackHoleProjectile::AAR_BlackHoleProjectile()
{
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComponent->SetupAttachment(RootComponent);
	
	// Using larger value for Impulse&ForceStrength if didn't implement URadialForceComponent::FireImpulse() in Tick()
	RadialForceComponent->ImpulseStrength = -800.0f;	
	RadialForceComponent->ForceStrength = -800.0f;	// Negative to pull in instead of push out
	RadialForceComponent->Radius = 800.0f;

	RadialForceComponent->SetAutoActivate(true);
	RadialForceComponent->bImpulseVelChange = true;
	RadialForceComponent->bIgnoreOwningActor = true;
	RadialForceComponent->bAutoActivate = false;
	// Avoid sucking player into the black hole
	RadialForceComponent->RemoveObjectTypeToAffect(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	// Suck up nearby objects, small enough to let them miss the sphere and flail around a bit first
	SphereComponent->SetSphereRadius(20.0f);
	// Profile to only overlap things like physics actors and never block on anything to pass through the world
	// OnProjHit() from base class will therefor never trigger as intended for this projectile
	SphereComponent->SetCollisionProfileName("BlackHoleCore");
	// Slow
	ProjectileMovementComponent->InitialSpeed = 800.0f;
	ProjectileMovementComponent->MaxSpeed = 1200.0f;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->bSweepCollision = false;
	
	InitialLifeSpan = 5.0f;
}

void AAR_BlackHoleProjectile::PostInitializeComponents()
{
	/** Note: Make sure GenerateOverlapEvents is enabled on the cubes in the world
	 * We have currently done this in Base class - Super::PostInitializeComponents(); */
	Super::PostInitializeComponents();
}

void AAR_BlackHoleProjectile::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Skip base implementation, we handle our own as we just need play looped VFX&SFX then 'suck' objects in black hole
	// Super::OnProjBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (IsValid(OtherActor) && OtherActor != InstigatorActorRef)
	{
		if (IsValid(OtherComp) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddRadialImpulse(GetActorLocation(), 200.0f, -200000.0f,
				RIF_Linear, true);
			OtherActor->Destroy();
		}
	}
	
}

void AAR_BlackHoleProjectile::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Super::OnProjHit(ComponentBeenHit, OtherActor, OtherComp, NormalImpulse, Hit);
	
}

void AAR_BlackHoleProjectile::PlayExplosionFXs_Implementation(const FHitResult& Hit)
{
	// No need to play Explosion VFX&SFX
	bExploded = true;
}
