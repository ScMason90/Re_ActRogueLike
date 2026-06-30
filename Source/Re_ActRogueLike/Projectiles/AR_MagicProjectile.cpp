// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_MagicProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_Effect.h"


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
	
	/* "Why damage applied through UE5 damage system like this won't trigger 'HealthOverwhelmed' problem?" 
	 Because UE5 merely provides interface implementations for causing/receiving damage, such as various 'ApplyDamage' 
	 and multicast dynamic delegation events. The changes related to 'HealthAmount' and the death issue caused by 'HealthOverwhelm', 
	 as well as other custom post-damage behaviors, are left for developers to decide on in UE5, 
	 and it has already provided the possible necessary parameters.*/ 
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