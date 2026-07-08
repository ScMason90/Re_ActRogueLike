// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_MagicProjectile.h"

// Necessary compilation header files
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_Effect.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



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
	
	if (EffectOnHit)
	{
		UAR_ActionSystemComponent* ASComp = OtherActor->FindComponentByClass<UAR_ActionSystemComponent>();
		if (ASComp)	// Ensure 'TargetActor' been hit has a one
		{
			ASComp->GrantAction(EffectOnHit);
		}	
	}
	
}