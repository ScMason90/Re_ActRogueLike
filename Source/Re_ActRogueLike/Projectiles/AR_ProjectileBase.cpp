// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ProjectileBase.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

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
	ProjectileMovementComponent->InitialSpeed = 2000.0f;
	
	LoopedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LoopedAudioComp"));
	LoopedAudioComponent->SetupAttachment(SphereComponent);	// Must SetupAttachment otherwise we can't edit in editor
	
	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;
	
	InitialLifeSpan = 30.0f;
}

void AAR_ProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// Don't use utilities 'SphereComponent->SetCollisionProfileName();' here 
	// if any derived cpp class desired to set a specified collision profile in constructor
	
	InstigatorActorRef = Cast<AActor>(GetInstigator());
	SphereComponent->IgnoreActorWhenMoving(InstigatorActorRef, true);
	
	SphereComponent->OnComponentHit.AddDynamic(this, &AAR_ProjectileBase::OnProjHit);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAR_ProjectileBase::OnProjBeginOverlap);
}

void AAR_ProjectileBase::OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleImpact(OtherComp, OtherActor, SweepResult);
}

void AAR_ProjectileBase::OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleImpact(OtherComp, OtherActor, Hit);
}

void AAR_ProjectileBase::HandleImpact(UPrimitiveComponent* OtherComp, AActor* OtherActor, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == InstigatorActorRef || bExploded)
		return;

	if (LoopedAudioComponent)LoopedAudioComponent->FadeOut(0.2f, 0.f);
	if (LoopedNiagaraComponent)LoopedNiagaraComponent->Deactivate();

	PlayExplosionFXs(Hit);
	OnImpact(OtherComp, OtherActor, Hit);

	Destroy();
}

void AAR_ProjectileBase::OnImpact(UPrimitiveComponent* OtherComp, AActor* OtherActor, const FHitResult& Hit)
{
	// The base class does no harm, while the subclass overrides
}

void AAR_ProjectileBase::PlayExplosionFXs_Implementation(const FHitResult& Hit)
{
	if (bExploded || IsPendingKillPending()) return;
	bExploded = true;
	
	FVector ProjExplodeLoc = GetActorLocation();
	// FRotator ProjExplodeRot = GetActorRotation();
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionVFX, ProjExplodeLoc);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, ProjExplodeLoc);
	UGameplayStatics::PlayWorldCameraShake(this, ImpactShake, ProjExplodeLoc, ImpactShakeInnerRadius, ImpactShakeOuterRadius);
}
