#include "AR_ProjectileBase.h"

#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AAR_ProjectileBase::AAR_ProjectileBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
    
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(SphereComp);
    
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.0f;
	MovementComp->InitialSpeed = 8000.0f;
	
	FlightAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("FlightAudioComp"));
	FlightAudioComp->SetupAttachment(SphereComp);	// Must SetupAttachment otherwise we can't edit in editor
	FlightAudioComp->bAutoActivate = false;
}

void AAR_ProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	SphereComp->OnComponentHit.AddDynamic(this, &AAR_ProjectileBase::OnProjHit);
}

void AAR_ProjectileBase::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlapping logic...
}

void AAR_ProjectileBase::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Here, the actual Hit is passed, not FHitResult().
	Explode(Hit);
}

void AAR_ProjectileBase::Explode_Implementation(const FHitResult& Hit)
{
	if (bExploded || IsPendingKillPending())
	{
		return;
	}
	bExploded = true;
	
	FVector ProjInsLocation = GetActorLocation();
	FRotator ProjInsRotation = GetActorRotation();

	if (IsValid(ImpactVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, ProjInsLocation, ProjInsRotation);
	}
	
	if (ImpactSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundCue, ProjInsLocation);
	}
	
	/* These could be not necessary for a base class implementation, may modularize in subclass.
	 * Noticed that we only need to 'Destroy()' actor then it will implement logic below */
	
	// if (EffectComp)EffectComp->DeactivateSystem();
	// if (MovementComp)MovementComp->StopMovementImmediately();
	// SetActorEnableCollision(false);
}
