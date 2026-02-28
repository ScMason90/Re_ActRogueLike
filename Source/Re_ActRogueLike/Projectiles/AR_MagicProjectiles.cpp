// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_MagicProjectiles.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
AAR_MagicProjectiles::AAR_MagicProjectiles()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InitialLifeSpan = 4.0f;
	
	// Collision sphere
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;
	
	// Visual effect
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(SphereComp);
	
	// Movement behavior
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	
	// Register hit event
	SphereComp->OnComponentHit.AddDynamic(this, &AAR_MagicProjectiles::OnActorHit);
}

void AAR_MagicProjectiles::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult)
{
	// Destroy projectile on impact
	Destroy();
}

// Called when the game starts or when spawned
void AAR_MagicProjectiles::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAR_MagicProjectiles::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

