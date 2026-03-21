// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ExplosiveBarrel.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "PhysicsEngine/RadialForceComponent.h"


void AAR_ExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Register hit event so the barrel reacts to physics collisions
	MeshComp->OnComponentHit.AddDynamic(this, &AAR_ExplosiveBarrel::OnActorHit);
	
	// Ensure the barrel is treated as a physics body
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
}

void AAR_ExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormaleImpulse, const FHitResult& Hit)
{
	// Apply explosion impulse
	ForceComp->FireImpulse();
	
	// Get readable names for debug output
	FString HitActorName = GetActorNameOrLabel()/* Name of the barrel instance in editor been hit */,
		OtherActorName = OtherActor ? OtherActor->GetActorNameOrLabel() : TEXT("Unknown");
	
	// Print collision info
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
		FString::Printf(TEXT("Actor %s was hit by %s"), *HitActorName, *OtherActorName));
}

// Sets default values
AAR_ExplosiveBarrel::AAR_ExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Mesh with physics simulation
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;
	
	// Radial force for explosion behavior
	ForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("ForceComp"));
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->SetAutoActivate(false);
	
	ForceComp->Radius = 750.0f;
	ForceComp->ImpulseStrength = 2500.0f;	// Alternative: 2000000.0f if bImpulseVelChange is false.
	// Optional, ignores 'Mass' of other objects 
	ForceComp->bImpulseVelChange = true;	//(if false, the impulse strength will be much higher to push most objects depending on Mass)
	
	// Optional, default constructor of component already has 4 object types to affect, exclude WorldDynamic.
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void AAR_ExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAR_ExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

