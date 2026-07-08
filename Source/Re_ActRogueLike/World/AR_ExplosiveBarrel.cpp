// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ExplosiveBarrel.h"

// Necessary compilation header files
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// Sets default values
AAR_ExplosiveBarrel::AAR_ExplosiveBarrel()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComponent->SetSimulatePhysics(true);
	// Blueprint automatically sets this collision profile when enabling simulate physics, in C++ we do this manually.
	MeshComponent->SetCollisionProfileName("PhysicsActor");
	RootComponent = MeshComponent;
	
	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComponent->SetupAttachment(MeshComponent);
	// No need to 'AutoActivate' this, that is only good for applying a constant radial force and not our desired one frame Impulse
	RadialForceComponent->SetAutoActivate(false);
	
	// Must be quite large, depends on the Mass of the Actors we intend to launch such as the Cubes
	RadialForceComponent->ImpulseStrength = 1500.0f;	// Alternative: 2000000.0f if bImpulseVelChange is false.
	RadialForceComponent->Radius = 750.0f;
	// Optional, ignores 'Mass' of other objects 
	RadialForceComponent->bImpulseVelChange = true;	//(if false, the impulse strength will be much higher to push most objects depending on Mass)
	
	// SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComp"));
	// SphereComponent->SetupAttachment(MeshComponent);
	// SphereComponent->SetSphereRadius(600.0f);
	// SphereComponent->SetCollisionProfileName("OverlapOnlyPawn");	//OverlapAllDynamic...
	// SphereComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	
}

void AAR_ExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Register hit event so the barrel reacts to physics collisions ->  Please note not to repeat with 'TakeDamage'
	// MeshComp->OnComponentHit.AddDynamic(this, &AAR_ExplosiveBarrel::OnActorHit);
	
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);	// Ensure the barrel is treated as a physics body
	
	// Optional, default constructor of component already has 4 object types to affect, exclude WorldDynamic.
	RadialForceComponent->AddCollisionChannelToAffect(ECC_WorldDynamic);
	
	RandomStream_ImpulseSelf.Initialize(4928651);
	
	// SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
	// SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	
}

void AAR_ExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	// UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), 
	// 	SphereComponent->GetScaledSphereRadius(), 
	// 	{UEngineTypes::ConvertToObjectType(ECC_Pawn)}, 
	// 	nullptr, {this}, ActorsOverlappedSphere);
	//
	// for (AActor* Actor : ActorsOverlappedSphere)
	// {
	// 	UE_LOG(LogGame, Warning, TEXT("AAR_ExplosiveBarrel::BeginPlay(), added overlapped actor %s"), *GetNameSafe(Actor));
	// }
}

// void AAR_ExplosiveBarrel::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	ActorsOverlappedSphere.Add(OtherActor);
// 	
// 	UE_LOG(LogGame, Warning, TEXT("AAR_ExplosiveBarrel::OnSphereBeginOverlap, added OtherActor %s"), *GetNameSafe(OtherActor));
// 	
// }
//
// void AAR_ExplosiveBarrel::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	ActorsOverlappedSphere.RemoveSingleSwap(OtherActor);
// 	
// 	UE_LOG(LogGame, Warning, TEXT("AAR_ExplosiveBarrel::OnSphereEndOverlap, Removed OtherActor %s"), *GetNameSafe(OtherActor));
// 	
// }

// TODO: Note that BP class instance derived from this cpp class can only 'explode' once
float AAR_ExplosiveBarrel::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	// Allow exploding once, also skip when timer is already running
	if (bExploded || GetWorldTimerManager().TimerExists(ExplosionTimerHandle)) return 0.0f;
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Hold onto both the Niagara and Audio Components to disable them during the Explode()
	ActiveBurningVFXComp = UNiagaraFunctionLibrary::SpawnSystemAttached(BurningVFX, MeshComponent, NAME_None,
		FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::SnapToTarget,true);
	ActiveBurningSFXComp = UGameplayStatics::SpawnSoundAttached(BurningSFX, MeshComponent);
	
	GetWorldTimerManager().SetTimer(ExplosionTimerHandle, this,  &AAR_ExplosiveBarrel::Explode, ExplosionDelay);
	
	return ActualDamage;
}

void AAR_ExplosiveBarrel::Explode()
{
	bExploded = true;
	
	// Crashed here if 'Active...VFX/SFXComp' is nullptr due to slow loading. Considering introduce some Async delegate callback?
	if (ActiveBurningVFXComp)ActiveBurningVFXComp->Deactivate();
	if (ActiveBurningSFXComp)ActiveBurningSFXComp->Stop();
	
	RadialForceComponent->FireImpulse();
	
	FVector ImpulseMain = (RandomStream_ImpulseSelf.GetUnitVector() + FVector::UpVector) * ImpulseSelfStrength * ImpulseSelfUpScalar;
	FVector ImpulseSideway = (RandomStream_ImpulseSelf.RandRange(0, 1) ? FVector::RightVector : FVector::LeftVector) * ImpulseSelfStrength * ImpulseSelfSideScalar;
	
	MeshComponent->AddImpulse(ImpulseMain, NAME_None, true);
	MeshComponent->AddAngularImpulseInDegrees(ImpulseSideway, NAME_None, true);
	
	// Apply damage to overlapped actor through sphere collision
	// for (AActor* OverlappedActor : ActorsOverlappedSphere)
	// {
	// 	UE_LOG(LogGame, Warning, TEXT("AAR_ExplosiveBarrel::Explode, apply damage to overlapped actor %s"), *GetNameSafe(OverlappedActor));
	// 	// if (OverlappedActor->CanBeDamaged())
	// 	{
	// 		UGameplayStatics::ApplyDamage(OverlappedActor, ExplosionDamage, GetInstigatorController(), this, DmgTypeClass);
	// 	}
	// }
	
	FVector BarrelLoc = GetActorLocation();
	FRotator BarrelRot = GetActorRotation();
	
	// TODO: Add a 'COLLISION_DAMAGE' TraceChannel in project collision settings in editor if got misc damage application or Real-time damage scenarios are becoming complex
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, ExplosionDamageAmount, ExplosionDamageAmount / 5.0f,
		BarrelLoc + FVector(0.0f, 0.0f, 30.0f), 300.0f, 1200.0f, 1.0f,
		DmgTypeClass, {this}, this, GetInstigatorController(), COLLISION_INTERACTION);
	
	
	// Deprecated...
	// if (!UGameplayStatics::ApplyRadialDamage(this, ExplosionDamageAmount, 
	// 	BarrelLoc + FVector(0.0f, 0.0f, 30.0f), 800.0f, DmgTypeClass, 
	// 	{this}, this, GetInstigatorController(), false, COLLISION_INTERACTION))
	// {
	// 	UE_LOG(LogGame, Warning, TEXT("AAR_ExplosiveBarrel::Explode, ApplyRadialDamage false!"));
	// }
	
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionVFX, BarrelLoc, BarrelRot);
	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSFX, BarrelLoc);
	
	// Hide barrel, diable 'TakeDamage' and manually unbind SphereComponent->Overlap dynamic event delegate
	SetCanBeDamaged(false);
	
	// SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ThisClass::OnSphereBeginOverlap);
	// SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &ThisClass::OnSphereEndOverlap);
	
	// You can create some physical effects of oil barrel exploding, shattering and damaging materials
	
	SetLifeSpan(7.0f);	// Delayed Destroy
}

void AAR_ExplosiveBarrel::LifeSpanExpired()
{
	MeshComponent->SetVisibility(false, true);
	
	Super::LifeSpanExpired();
}