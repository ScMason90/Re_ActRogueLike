#include "AR_ProjectileBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Camera/CameraShakeBase.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

AAR_ProjectileBase::AAR_ProjectileBase()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComponent->SetSphereRadius(16.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = SphereComponent;
    
	LoopedNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LoopedNiagaraComp"));
	LoopedNiagaraComponent->SetupAttachment(SphereComponent);
    
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->InitialSpeed = 8000.0f;
	
	LoopedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LoopedAudioComp"));
	LoopedAudioComponent->SetupAttachment(SphereComponent);	// Must SetupAttachment otherwise we can't edit in editor
	
	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;
}

void AAR_ProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Don't use utilities 'SphereComponent->SetCollisionProfileName();' here 
	// if any derived cpp class desired to set a specified collision profile in constructor
	
	InstigatorPawnActorRef = Cast<AActor>(GetInstigator());
	SphereComponent->IgnoreActorWhenMoving(InstigatorPawnActorRef, true);
	
	SphereComponent->OnComponentHit.AddDynamic(this, &AAR_ProjectileBase::OnProjHit);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAR_ProjectileBase::OnProjBeginOverlap);
}

void AAR_ProjectileBase::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleImpact(OtherActor, SweepResult);
}

void AAR_ProjectileBase::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleImpact(OtherActor, Hit);
}

void AAR_ProjectileBase::PlayExplodeVFXandSFX(FVector const& ProjInsLocation, FRotator const& ProjInsRotation)
{
	if (IsValid(ExplosionVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, ExplosionVFX, ProjInsLocation, ProjInsRotation);
	}
	
	if (IsValid(ExplosionSFX))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, ProjInsLocation);
	}
	
	if (IsValid(ImpactShake))
	{
		UGameplayStatics::PlayWorldCameraShake(
			this, ImpactShake, ProjInsLocation, ImpactShakeInnerRadius, ImpactShakeOuterRadius);
	}
}

void AAR_ProjectileBase::HandleImpact(AActor* OtherActor, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == InstigatorPawnActorRef || bExploded)
		return;

	if (LoopedAudioComponent)LoopedAudioComponent->FadeOut(0.2f, 0.f);
	if (LoopedNiagaraComponent)LoopedNiagaraComponent->Deactivate();

	Explode(Hit);
	OnImpact(OtherActor, Hit);

	Destroy();
}

void AAR_ProjectileBase::OnImpact(AActor* OtherActor, const FHitResult& Hit)
{
	// The base class does no harm, while the subclass overrides
}


void AAR_ProjectileBase::Explode_Implementation(const FHitResult& Hit)
{
	if (bExploded || IsPendingKillPending())
	{
		return;
	}
	bExploded = true;
	
	LoopedAudioComponent->Deactivate();
	// LoopedNiagaraComponent->Deactivate();
	PlayExplodeVFXandSFX(GetActorLocation(), GetActorRotation());
}
