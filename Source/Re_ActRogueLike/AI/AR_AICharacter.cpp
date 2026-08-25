// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AICharacter.h"

// Necessary compilation header files
#include "AR_AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"
#include "Re_ActRogueLike/Core/AR_GameInstance.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/UI/AR_WorldUserWidget.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// Sets default values
AAR_AICharacter::AAR_AICharacter()
{
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	ActionSystemComponent->SetDefaultAttributeSet(UAR_EnemyAttributeSet::StaticClass());
	
	AIControllerClass = AAR_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

FGenericTeamId AAR_AICharacter::GetGenericTeamId() const
{
	if (AAIController* AIC = GetController<AAIController>())
	{
		return AIC->GetGenericTeamId();
	}
	
	return FGenericTeamId::NoTeam;
}

void AAR_AICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	InitializeMIDs();
	
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
	
	UAR_GameplayStatics_BIND_ATTR_MULTICAST(
		this, &AAR_AICharacter::OnHealthChanged, ActionSystemComponent, SharedGameplayTags::Attribute_Health);
	
	ActionSystemComponent->OnGameplayTagCountUpdated.AddDynamic(this, &ThisClass::AAR_AICharacter::OnGameplayTagCountUpdated);
	
	// When debugging or testing BehaviorTree, ensure AutoPossessAI corresponding your way of 'run' AIChar BP class.
}

// Sanity cache BTComp and AIC safely for convenience.
// void ARogueAICharacter::PossessedBy(AController* NewController)
// {
// 	Super::PossessedBy(NewController);
//
// 	CachedAIController = Cast<AAIController>(NewController);
// 	CachedBTComp = Cast<UBehaviorTreeComponent>(CachedAIController->GetBrainComponent());
// }

void AAR_AICharacter::OnGameplayTagCountUpdated(FGameplayTag UpdatedTag, int32 NewCount)
{
	if (bAIPawnDying) return;	// We don't wanna apply any 'ActionEffect' when processing dead logic
	
	const bool bWasAdded = NewCount > 0;
	
	// Stun
	if (UpdatedTag.MatchesTag(SharedGameplayTags::StatusEffect_Stunned))
	{
		GetCharacterMovement()->SetMovementMode(bWasAdded ? MOVE_None : MOVE_Walking);	// Block/Allow Movement
		
		// Pause All logic for Enemy
		AAR_AIController* AIC = Cast<AAR_AIController>(GetController());
		check(AIC);	// Could be nullptr if AICharacter didn't 'OnPossessedBy()' yet when new spawned
		UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIC->GetBrainComponent());
		check(BTComp);
		
		/* Alternatively you could restart the logic to skip any 'waits' it might be in and get out of the stunned
		 * picking a new thing to do right away...((BTComp->StartLogic() BTComp->StopLogic())) */ 
		if (bWasAdded)
		{
			BTComp->StopLogic("StunApplied");
			
			// Animation
			PlayAnimMontage(StunnedMontage);
		}
		else
		{	
			// Might directly freeze after 'StunEnded' since AIChar doesn't "actively seek out available targets" now
			BTComp->ResumeLogic("StunRemoved");
			BTComp->StartLogic();	// Uncomment this if temporarily want MinionRanged to behavior after Stunned
		}
	}
	
}

void AAR_AICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UAR_GameInstance* GI = GetGameInstance<UAR_GameInstance>();	// Cache this pointer as a global variable?
	GI->AliveEnemies.Add(this);
}

void AAR_AICharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UAR_GameInstance* GI = GetGameInstance<UAR_GameInstance>();
	GI->AliveEnemies.RemoveSingleSwap(this);
}

float AAR_AICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                  class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ActualDamage *= UAR_GameplayStatics::GetDmgModifier();	// Apply 'DamageMultiplier'
	
	const ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*EventInstigator);
	if (IsValid(EventInstigator) && Attitude != ETeamAttitude::Friendly)
	{
		UAISense_Damage::ReportDamageEvent(this, this, EventInstigator->GetPawn(),
			FMath::Abs(ActualDamage), EventInstigator->GetPawn()->GetActorLocation(), GetActorLocation());
	}
	
	// Implement GameplayTag for BP class to match. e.g. Ignore damage if DamageCauser is friendly
	// Target the 'InstigatorActor' who damaged self.
	if (EventInstigator && DamageCauser)
	{
		// Target switching when new damaged should be designed better participating in a Hatred System. 
		if (AAR_AIController* AICon = Cast<AAR_AIController>(GetController())) 
			AICon->SetTargetActorBB(DamageCauser);
	}
	
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	
	return ActualDamage;
}

void AAR_AICharacter::OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth)
{
	if (bAIPawnDying) return;
	
	// Death checkpoint
	if (const bool bIsDying = UAR_GameplayStatics::IsDying(ActionSystemComponent))
	{
		bAIPawnDying = bIsDying;
		HandleDeath();
		return;		// Remove this line, and migrate this snippet under 'HandleDamaged()' if you want both.
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
				ActiveHealthBar->AddToViewport();	// More UMG logic within cpp To be continued...
			}
		}
		
		// HitFlashOverlay - Note: this design was rough...'demo/tutorial' only
		GetMesh()->SetOverlayMaterialMaxDrawDistance(0);
		GetMesh()->SetCustomPrimitiveDataFloat(0, GetWorld()->TimeSeconds);
		
		// HitFlashOverlay Ended 
		GetWorldTimerManager().SetTimer(TimerHandle_Overlay, [this]()
		{
			GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
		}, 1.0f/* Overlay Flash Duration*/, false);
		
	}
	
}

void AAR_AICharacter::HandleDeath()
{
	// End up all actions and effects of ActionSystemComponent
	ActionSystemComponent->EndActionsAndEffects();
	
	// Remove multicast attribute delegate.
	UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(ActionSystemComponent, SharedGameplayTags::Attribute_Health, DelHandle_OnHealthChanged);
	
	// If you need to "update the number of enemies" immediately...Defensive programming but repeats in EndPlay() call
	// UAR_GameInstance* GI = GetGameInstance<UAR_GameInstance>();
	// GI->AliveEnemies.RemoveSingle(this);
	
	// Disable AI Behavior Tree
	AAR_AIController* AIController = GetController<AAR_AIController>();
	check(AIController->GetBrainComponent());	// Usually won't trigger this line?
	AIController->GetBrainComponent()->StopLogic("Dead");
	
	USkeletalMeshComponent* MeshComp = GetMesh();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	
	// Disable Collision...Honestly all post-death appearances depend on your game type/design
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Disable Movement
	GetMovementComponent()->StopActiveMovement();	// GetCharacterMovement()->DisableMovement();
		
	// Play a noob Death Anim
	PlayAnimMontage(DeathMontage);
	
	FTimerDelegate RagdollDelegate;
	RagdollDelegate.BindWeakLambda(this, [this, MeshComp, CapsuleComp]()
	{
		if (!IsValid(this) || IsPendingKillPending()) return;
		
		MeshComp->bPauseAnims = true;
		// Optional - Play ragdoll
		MeshComp->SetAllBodiesSimulatePhysics(true);
		MeshComp->SetCollisionProfileName("Ragdoll");	
		
		// Set "Ragdoll" response to Object Type 'Projectile' as 'Ignore' in ProjectSettings if you want?
		MeshComp->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Ignore);
		
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
