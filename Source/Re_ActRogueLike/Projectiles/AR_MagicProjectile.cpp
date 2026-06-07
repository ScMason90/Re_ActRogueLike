// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_MagicProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AAR_MagicProjectile::AAR_MagicProjectile()
{
	SphereComponent->SetSphereRadius(20.0f);
	
	ProjectileMovementComponent->InitialSpeed = 2000.0f;

	DamageAmount = 3.0f;
	InitialLifeSpan = 4.0f;
}

void AAR_MagicProjectile::LifeSpanExpired()
{
	PlayExplosionFXs(FHitResult());
	Super::LifeSpanExpired();
}

void AAR_MagicProjectile::OnImpact(AActor* OtherActor, const FHitResult& Hit)
{
	Super::OnImpact(OtherActor, Hit);
	
	FVector HitFromDirection = GetActorRotation().Vector();
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, HitFromDirection, Hit, 
		InstigatorActorRef->GetInstigatorController(), this, DmgTypeClass);
	
}
