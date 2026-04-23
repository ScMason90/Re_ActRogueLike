// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AICharacter.h"

#include "AR_AIController.h"
#include "TimerManager.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

// Sets default values
AAR_AICharacter::AAR_AICharacter()
{
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	TimeToHitParamName = "TimeToHit";
}

void AAR_AICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ActionSystemComponent->OnHealthChanged.AddDynamic(this, &AAR_AICharacter::OnHealthChanged);
	
	InitializeMIDs();
}

void AAR_AICharacter::BeginPlay()
{
	Super::BeginPlay();
}

float AAR_AICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ActionSystemComponent->ApplyHealthChange(DamageCauser, -ActualDamage);
	
	return ActualDamage;
}

void AAR_AICharacter::OnHealthChanged(AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth,
                                      float Delta)
{
	if (AIPawnDying) return;
	if (OwningComp->IsDead())
	{
		AIPawnDying = OwningComp->IsDead();	// Marked as already dead
		HandleDeath();
		return;
	}
	
	// Flash when damaged - PS:this design was too rough
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		GEngine->AddOnScreenDebugMessage(01, 3, FColor::Emerald, 
			FString::Printf(
				TEXT("AAR_AICharacter::OnHealthChanged(), Current health: %.2f"), ActionSystemComponent->GetHealth()));
	}
}

void AAR_AICharacter::HandleDeath()
{
	// Disable AI Behavior Tree
	if (AAR_AIController* AIController = Cast<AAR_AIController>(GetController()))
	{
		if (UBehaviorTreeComponent* AI_BT = AIController->FindComponentByClass<UBehaviorTreeComponent>())
		{
			AI_BT->StopTree(EBTStopMode::Safe);
		}
		AIController->StopMovement();
		AIController->UnPossess();	// Optional?
	}
	// Disable Collision...
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Disable Movement
	GetMovementComponent()->StopActiveMovement();
		
	// Play Death Anim in Anim Class of PlayerCharacter...
	PlayAnimMontage(DeathMontage);
			
	// Optional
	// --Play ragdoll--	
	// GetMesh()->SetSimulatePhysics(true);
	// --Delayed destruction--
	// SetLifeSpan(5.0f);
			
	// Dissolve mesh material and destroy current AR_AICharacter instance
	StartDissolve();
}

void AAR_AICharacter::InitializeMIDs()
{
	int32 Count = GetMesh()->GetNumMaterials();
	for (int32 i = 0; i < Count; i++)
	{
		UMaterialInstanceDynamic* MID = GetMesh()->CreateAndSetMaterialInstanceDynamic(i);
		DynamicMIDs.Add(MID);
		// Initialize 'Dissolve' parameters, actually it already set in MF
		MID->SetScalarParameterValue("DissolveAmount", 0.0f);
		MID->SetScalarParameterValue("TimeToHit", -10.0f);
	}
}

void AAR_AICharacter::StartDissolve()
{
	DissolveAmount = 0.0f;
	
	GetWorldTimerManager().SetTimer(TimerHandle_Dissolve, this, 
		&AAR_AICharacter::UpdateDissolve, DissolveLoopRate, true);
}

void AAR_AICharacter::UpdateDissolve()
{
	DissolveAmount += DissolveRate;
	
	for (UMaterialInstanceDynamic* MID : DynamicMIDs) MID->SetScalarParameterValue("DissolveAmount", DissolveAmount);
	
	if (DissolveAmount >= 1.0f)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Dissolve);
		Destroy();
	}
}
