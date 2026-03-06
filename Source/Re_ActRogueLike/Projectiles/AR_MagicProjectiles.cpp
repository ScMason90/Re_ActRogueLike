// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_MagicProjectiles.h"

#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
	// SphereComp->OnComponentHit.AddDynamic(this, &AAR_MagicProjectiles::OnActorHit);
}

// void AAR_MagicProjectiles::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("HitComponent: %s"), *HitComponent->GetName());
// 	UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *GetNameSafe(OtherActor));
// 	UE_LOG(LogTemp, Warning, TEXT("NormalImpulse: %s"), *NormalImpulse.ToString());
// 	UE_LOG(LogTemp, Warning, TEXT("HitLocation: %s"), *Hit.ImpactPoint.ToString());
//
// 	
// 	// Destroy projectile on impact
// 	Explode(&Hit);
// }

void AAR_MagicProjectiles::LifeSpanExpired()
{
	Explode(nullptr);
	Super::LifeSpanExpired();
}

void AAR_MagicProjectiles::Explode(const FHitResult* Hit)
{
	if (ImpactVFX)
	{
		if (Hit) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, 
			Hit->ImpactPoint, Hit->ImpactNormal.Rotation());
		else UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation());
	}
	
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

