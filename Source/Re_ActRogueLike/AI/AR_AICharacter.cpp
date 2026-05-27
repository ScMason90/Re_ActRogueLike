// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AICharacter.h"

#include "AR_AIController.h"
#include "TimerManager.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/UI/AR_WorldUserWidget.h"

// Sets default values
AAR_AICharacter::AAR_AICharacter()
{
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	ActionSystemComponent->SetDefaultAttributeSet(UAR_EnemyAttributeSet::StaticClass());
	
	TimeToHitParamName = "TimeToHit";
	
	AIControllerClass = AAR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAR_AICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	InitializeMIDs();
	
	FOnAttributeChanged& Event = ActionSystemComponent->GetAttributeListener(SharedGameplayTags::Attribute_Health);
	Event.AddUObject(this, &ThisClass::OnHealthChanged);
}

void AAR_AICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

float AAR_AICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// Target the 'InstigatorActor' who damaged self.
	if (AAR_AIController* AICon = Cast<AAR_AIController>(GetController()))
		AICon->SetTargetActor(DamageCauser);
	
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	
	return ActualDamage;
}

void AAR_AICharacter::OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	if (bAIPawnDying) return;
	if (const bool bIsDead = UAR_GameplayStatics::IsDead(ActionSystemComponent))
	{
		bAIPawnDying = bIsDead;	// Marked as already dead
		HandleDeath();
		return;
	}
	
	// 'HandleDamaged()'?
	if (const float Delta = NewHealth - OldHealth; Delta < 0.0f)
	{
		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<UAR_WorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();	// So, we could do some UMG logic in CPP rather in BP?
			}
		}
		
		// Flash when damaged - PS:this design was too rough
		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		// GEngine->AddOnScreenDebugMessage(01, 3, FColor::Emerald, 
		// 	FString::Printf(TEXT("AAR_AICharacter::OnHealthChanged(), Current health: %.2f"), 
		// 		ActionSystemComponent->GetAttribute(SharedGameplayTags::Attribute_Health)->GetValue()));
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
	USkeletalMeshComponent* MeshComp = GetMesh();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	
	// Disable Collision...Honestly all post-death appearances depend on your game type/design
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Disable Movement
	GetMovementComponent()->StopActiveMovement();
		
	// Play a noob Death Anim
	PlayAnimMontage(DeathMontage);
	
	FTimerDelegate RagdollDelegate;
	RagdollDelegate.BindWeakLambda(this, [this, MeshComp, CapsuleComp]()
	{
		if (!IsValid(this) || IsPendingKillPending()) return;
		
		// Optional
		// --Play ragdoll--	
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetAllBodiesSimulatePhysics(true);
		MeshComp->SetCollisionProfileName("Ragdoll");
		
		// Dissolve mesh material and destroy current AR_AICharacter instance
		StartDissolve();
	});
	FTimerHandle TimerHandle_Ragdoll;
	GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, RagdollDelegate, DeathAnimDuration, false);
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
		SetLifeSpan(3.0f);	// 'Delayed Destruction'...
	}
}
