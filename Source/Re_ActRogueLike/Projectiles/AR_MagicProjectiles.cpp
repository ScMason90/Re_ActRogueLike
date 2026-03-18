#include "AR_MagicProjectiles.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"

AAR_MagicProjectiles::AAR_MagicProjectiles()
{
	SphereComp->SetSphereRadius(20.0f);

	if (MovementComp)
	{
		MovementComp->InitialSpeed = 1000.0f;
	}

	DamageAmount = -20.0f;
	InitialLifeSpan = 4.0f;

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAR_MagicProjectiles::OnComponentBeginOverlap);
}

void AAR_MagicProjectiles::LifeSpanExpired()
{
	Explode(FHitResult());
	Super::LifeSpanExpired();
}

void AAR_MagicProjectiles::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* InstigatorActor = GetInstigator();

	// Change BP_MagicProjectile's collision response to 'Overlap' to Pawn for testing snippet under
	if (OtherActor && OtherActor != Cast<AActor>(InstigatorActor))
	{
		if (UAR_AttributeComponent* AttributeComp = Cast<UAR_AttributeComponent>(
			OtherActor->GetComponentByClass(UAR_AttributeComponent::StaticClass())))
		{
			AttributeComp->ApplyHealthChange(DamageAmount);
			Explode(SweepResult);
			Destroy();
		}
	}
}
