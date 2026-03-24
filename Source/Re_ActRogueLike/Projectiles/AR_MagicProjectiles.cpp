#include "AR_MagicProjectiles.h"

#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
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

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AAR_MagicProjectiles::OnProjBeginOverlap);
}

void AAR_MagicProjectiles::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	InstigatorPawnActorRef = Cast<AActor>(GetInstigator());
	
	SphereComp->IgnoreActorWhenMoving(InstigatorPawnActorRef, true);
}

// Called when the game starts or when spawned
void AAR_MagicProjectiles::BeginPlay()
{
	Super::BeginPlay();
	
	FlightAudioComp->Play();
}

void AAR_MagicProjectiles::LifeSpanExpired()
{
	Explode(FHitResult());
	Super::LifeSpanExpired();
}

void AAR_MagicProjectiles::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Change BP_MagicProjectile's collision response to 'Overlap' to Pawn for testing snippet under
	if (OtherActor && OtherActor != InstigatorPawnActorRef)
	{
		if (UAR_AttributeComponent* AttributeComp = Cast<UAR_AttributeComponent>(
			OtherActor->GetComponentByClass(UAR_AttributeComponent::StaticClass())))
		{
			AttributeComp->ApplyHealthChange(DamageAmount);
			Explode(SweepResult);
			
			if (AttributeComp->IsDead()) SphereComp->SetCollisionProfileName("IgnoreOnlyPawn");
			if (IsValid(this))Destroy();
		}
	}
}

void AAR_MagicProjectiles::OnProjHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalisedImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != InstigatorPawnActorRef)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 10.0f, 12, FColor::Red,
			false, 1.3f, 0, 0.3f);
		
		FlightAudioComp->FadeOut(0.2f, 0.0f);
		Explode_Implementation(FHitResult());
		
		if (IsValid(this))Destroy();
	}
}
