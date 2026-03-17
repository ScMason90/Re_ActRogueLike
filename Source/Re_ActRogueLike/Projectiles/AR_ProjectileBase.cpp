#include "AR_ProjectileBase.h"
#include "Kismet/GameplayStatics.h"

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
    
	SphereComp->SetCollisionProfileName(TEXT("Projectile"));
	SphereComp->OnComponentHit.AddDynamic(this, &AAR_ProjectileBase::OnProjHit);
}

void AAR_ProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void AAR_ProjectileBase::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 这里传真正的 Hit，而不是 FHitResult()
	Explode(Hit);
}

void AAR_ProjectileBase::Explode_Implementation(const FHitResult& Hit)
{
	if (bExploded || IsPendingKillPending())
	{
		return;
	}
	bExploded = true;

	if (ImpactVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation(), GetActorRotation());
	}

	if (EffectComp)
	{
		EffectComp->DeactivateSystem();
	}

	if (MovementComp)
	{
		MovementComp->StopMovementImmediately();
	}

	SetActorEnableCollision(false);

	// 基类只负责「死相」，不强制 Destroy
	// 由子类决定是立刻 Destroy 还是延迟/Teleport 等
}
