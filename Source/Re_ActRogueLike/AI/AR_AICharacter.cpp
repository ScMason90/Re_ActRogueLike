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
	if (!IsPlayerDead)
	{
		// Flash when damaged - PS:this design was too rough
		if (Delta < 0.0f)
		{
			GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
			GEngine->AddOnScreenDebugMessage(01, 3, FColor::Emerald, 
				FString::Printf(
					TEXT("AAR_AICharacter::OnHealthChanged(), Current health: %.2f"), ActionSystemComponent->GetHealth()));
		}
	
		// TODO:We should implement death logic with specified relative behavior tree
		if (FMath::IsNearlyZero(NewHealth)/*NewHealth <= 0.0f*/)
		{
			// Mark as Dead
			IsPlayerDead = ActionSystemComponent->IsDead();
		
			// Disable AI Behavior Tree
			if (AAR_AIController* AIController = Cast<AAR_AIController>(GetController()))
			{
				if (UBehaviorTreeComponent* AI_BT = AIController->FindComponentByClass<UBehaviorTreeComponent>())
				{
					AI_BT->StopTree(EBTStopMode::Safe);
				}
				AIController->StopMovement();
				// Optional
				AIController->UnPossess();
			}
		
			// Disable Movement
			GetMovementComponent()->StopActiveMovement();
		
			// Play Death Anim in Anim Class of PlayerCharacter...
			PlayAnimMontage(DeathMontage);
		
			// Disable Collision...
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			// Optional
			// --Play ragdoll--	
			// GetMesh()->SetSimulatePhysics(true);
			// --Delayed destruction--
			// SetLifeSpan(5.0f);
			
			// Dissolve mesh material and destroy current AR_AICharacter instance
			// done in bp.Considering moved here?
		}
	}
}
