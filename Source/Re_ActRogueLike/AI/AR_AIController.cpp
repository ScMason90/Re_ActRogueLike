// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AIController.h"

// Necessary compilation header files
#include "AR_AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// Sets default values
AAR_AIController::AAR_AIController()
{
	
}

// Called when the game starts or when spawned
void AAR_AIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensureAlwaysMsgf(BehaviorTree, TEXT("AAR_AIController::BeginPlay(), BehaviorTree is nullptr, please assign it")))return;
	
	RunBehaviorTree(BehaviorTree);
	
	FName TargetActor = NAME_TargetActor, MoveToLocation = FName("MoveToLocation");
	
	// Temporarily hard-coded only for local single play and early learning. 
	LocalPlayerRef = UGameplayStatics::GetPlayerPawn(this, 0);
	// if (!LocalPlayerRef.IsValid()) return;	// Do not trigger fatal-assert if you want run DebugGame after 'LocalPlayer' dead
	check(LocalPlayerRef.Get());	// This's wrong behavior when introduce Multiplay / Server network replication.
	
	GetBlackboardComponent()->SetValueAsVector(MoveToLocation, LocalPlayerRef->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TargetActor, LocalPlayerRef.Get());
}

void AAR_AIController::SetTargetActorBB(AActor* NewTarget)
{
	if (NewTarget->IsPendingKillPending() || !IsValid(NewTarget)) return;	// Ignore if invalid
	TargetActorASComp = NewTarget->FindComponentByClass<UAR_ActionSystemComponent>();
	if (!TargetActorASComp.IsValid()) return;
	
	// UE_LOG(LogController, Log, TEXT("AAR_AIController::SetTargetActorBB, %s of %s"), *GetNameSafe(TargetActorASComp), *GetNameSafe(NewTarget));
	
	UBlackboardComponent* BB = GetBlackboardComponent();	// Switch target only NEW
	if (!BB || BB->GetValueAsObject(NAME_TargetActor) == NewTarget) return;
	
	// Survival check
	if (UAR_GameplayStatics::IsDying(TargetActorASComp.Get()))
	{
		UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(
			TargetActorASComp.Get(), SharedGameplayTags::Attribute_Health, DelHandle_OnTargetActorHealthChanged);
		return;
	}
		
	// Subscribed for health changed event of 'NewTarget'
	DelHandle_OnTargetActorHealthChanged = UAR_GameplayStatics_BIND_ATTR_MULTICAST(
		this, &AAR_AIController::OnTargetActorHealthChanged, TargetActorASComp.Get(), SharedGameplayTags::Attribute_Health);
	// UE_LOG(LogController, Log, TEXT("AAR_AIController::SetTargetActorBB, DelHandle_OnTargetActorHealthChanged ? %d"), DelHandle_OnTargetActorHealthChanged.IsValid());

	BB->SetValueAsObject(NAME_TargetActor, NewTarget);
	SetFocus(NewTarget, EAIFocusPriority::Gameplay);

	StopMovement();   // Optional, according to needs

	// UE_LOG(LogGame, Log, TEXT("AAR_AIController::SetTargetActorBB -> %s"), *NewTarget->GetActorNameOrLabel());
}

void AAR_AIController::OnTargetActorHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	if (FMath::IsNearlyZero(NewHealth))
	{
		UBlackboardComponent* BB = GetBlackboardComponent();
		if (!BB) return;
		
		//...Find/Switch to another alive player as new target, actively bind its health changed ?
		// Cancel Targeting, set to default
		BB->SetValueAsObject(NAME_TargetActor, LocalPlayerRef.Get());
		SetFocus(LocalPlayerRef.Get(), EAIFocusPriority::Gameplay);
		
		UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(
			TargetActorASComp.Get(), SharedGameplayTags::Attribute_Health, DelHandle_OnTargetActorHealthChanged);
		
		
	}
	
	// ...
	
}
