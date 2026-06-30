// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Action_ProjectileAttack.h"

#include "AR_ActionSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/Projectiles/AR_ProjectileBase.h"

static TAutoConsoleVariable<float> CVarProjectileAdjustmentDebugDrawing(TEXT("game.projectile.DebugDraw"), 0.0f,
	TEXT("Enable projectile aim adjustment debug rendering. (0 = off, > 0 is duration)"), ECVF_Cheat);

UAR_Action_ProjectileAttack::UAR_Action_ProjectileAttack()
{
	MuzzleSocketName = "Muzzle_01";
	LineTraceEndOffset = 10000.0f;
	FireDelay = 0.2f;
	CooldownTime = 0.5f;
}

void UAR_Action_ProjectileAttack::StartAction_Implementation()
{
	Super::StartAction_Implementation();
	
	UAR_ActionSystemComponent* ASComp = GetOwningASComponent();
	ACharacter* Character = CastChecked<ACharacter>(ASComp->GetOwner());
	
	Character->PlayAnimMontage(FireMontage);
	
	UNiagaraFunctionLibrary::SpawnSystemAttached(CastingVFX, Character->GetMesh(), 
		MuzzleSocketName, FVector::ZeroVector, FRotator::ZeroRotator, 
		EAttachLocation::SnapToTarget, true);
	UGameplayStatics::PlaySound2D(this, CastingSFX);
	
	FTimerHandle TimerHandle_FireProj;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireProj, 
		[this, Character]()/*FireProjTimeElapsed()*/
		{
			FTransform SpawnTM = AdjustedProjSpawnTransform(*Character, MuzzleSocketName, LineTraceEndOffset);
	
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = Character;

			AActor* NewProjectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
			Character->MoveIgnoreActorAdd(NewProjectile);
			
			StopAction();
			
		}, FireDelay, false);
}

FTransform UAR_Action_ProjectileAttack::AdjustedProjSpawnTransform(
	ACharacter& Character, FName InSocketName, float InLineTraceEndOffset)
{
	FVector TraceStart/*EyeLocation*/;
	FRotator EyeRotation;
	Character.GetController()->GetPlayerViewPoint(TraceStart, EyeRotation);
	
	FVector HandLocation = Character.GetMesh()->GetSocketLocation(InSocketName);
	FVector TraceEnd = TraceStart + (EyeRotation.Vector() * InLineTraceEndOffset);
	FVector AimShot = TraceEnd;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(&Character);
	
	FHitResult Hit;
	UWorld* World = GetWorld();
	/* Legacy bug - Wrong behavior when LineTrace 'Hit' any actor derived from class AR_MagicProjectile,
	 * It will cause proj actor spawn into an odd direction.May need fix it with specified Trace Channel
	 * ↑ Nah, after 2h struggling on collision channel/preset/object type.I initially repair it.--26.3.13*/ 
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, AimShot, COLLISION_PROJECTILE, QueryParams))
	{
		AimShot = Hit.ImpactPoint;
	}
	/* TODO: Spawn MagicProj in a position which camera was too close to something,
	 * The proj will go to the item that closely block around camera. This's not a stable trace adjustment when
	 * third person camera is close to player character.*/
	FQuat UnderCrossHairQuat = (AimShot - HandLocation).GetSafeNormal().ToOrientationQuat();
	FTransform SpawnTM;
	SpawnTM.SetLocation(HandLocation);
	SpawnTM.SetRotation(UnderCrossHairQuat);
	
#if !UE_BUILD_SHIPPING
	float DebugDrawDuration = CVarProjectileAdjustmentDebugDrawing.GetValueOnGameThread();
	if (DebugDrawDuration > 0.0f)
	{
		// The hit location or trace end
		DrawDebugBox(World, AimShot, FVector(20.0f), FColor::Green, false, DebugDrawDuration);
		
		// Adjustment line trace
		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Green, false, DebugDrawDuration);
		
		// New projectile path
		DrawDebugLine(World, HandLocation, AimShot, FColor::Yellow, false, DebugDrawDuration);
		
		// The original path of the projectile
		DrawDebugLine(World, HandLocation, HandLocation + (EyeRotation.Vector() * 5000.0f), 
			FColor::Purple, false, DebugDrawDuration);
	}
#endif
	
	return SpawnTM;
}
