#include "AR_MagicProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"

AAR_MagicProjectile::AAR_MagicProjectile()
{
	SphereComponent->SetSphereRadius(20.0f);
	
	ProjectileMovementComponent->InitialSpeed = 2000.0f;

	DamageAmount = 10.0f;
	InitialLifeSpan = 4.0f;
}

void AAR_MagicProjectile::LifeSpanExpired()
{
	Explode(FHitResult());
	Super::LifeSpanExpired();
}

void AAR_MagicProjectile::OnImpact(AActor* OtherActor, const FHitResult& Hit)
{
	Super::OnImpact(OtherActor, Hit);
	
	FVector HitFromDirection = GetActorRotation().Vector();
	UGameplayStatics::ApplyPointDamage(OtherActor, DamageAmount, HitFromDirection, Hit, 
		InstigatorPawnActorRef->GetInstigatorController(), this, DmgTypeClass);
	
	// Legacy Damage Logic
	if (OtherActor && OtherActor != InstigatorPawnActorRef)
	{
		if (UAR_AttributeComponent* AttributeComp = Cast<UAR_AttributeComponent>(
			OtherActor->GetComponentByClass(UAR_AttributeComponent::StaticClass())))
		{
			AttributeComp->ApplyHealthChange(-DamageAmount);
			Explode(Hit);
			if (IsValid(this))Destroy();
		}
	}
}
