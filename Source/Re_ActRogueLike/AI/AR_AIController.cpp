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
#include "Re_ActRogueLike/Projectiles/AR_ProjectileBase.h"

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
	check(LocalPlayerRef.Get());	// This's wrong behavior when introduce Multiplay / Server network replication.
	
	GetBlackboardComponent()->SetValueAsVector(MoveToLocation, LocalPlayerRef->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TargetActor, LocalPlayerRef.Get());
}

void AAR_AIController::SetTargetActorBB(AActor* NewTarget)
{
	if (!IsValid(NewTarget))return;

	UBlackboardComponent* BB = GetBlackboardComponent();	// Switch target only when current dying
	if (!BB || IsValid(BB->GetValueAsObject(NAME_TargetActor))) return;
	
	AActor* ValidTarget = NewTarget;
	TargetActorASComp = ValidTarget->FindComponentByClass<UAR_ActionSystemComponent>();
	
	if (NewTarget->IsPendingKillPending() || !IsValid(NewTarget) || NewTarget->IsA(AAR_ProjectileBase::StaticClass()))
	{
		ValidTarget = NewTarget->GetInstigator();
	}
	
	// UE_LOG(LogController, Log, TEXT("AAR_AIController::SetTargetActor, %s of %s"), *GetNameSafe(TargetActorASComp), *GetNameSafe(ValidTarget));
	
	if (TargetActorASComp.Get())
	{
		FOnAttributeChanged& TargetHealthChangedEvent = 
			TargetActorASComp->GetAttributeListener(SharedGameplayTags::Attribute_Health);
		
		// Survival check
		if (UAR_GameplayStatics::IsDying(TargetActorASComp.Get()))
		{
			TargetHealthChangedEvent.Remove(DelHandle_OnTargetActorHealthChanged);
			return;
		}
		
		// Delegate Subscribed 'Health == 0.0f'-'IsDying' event of TargetActor
		DelHandle_OnTargetActorHealthChanged = TargetHealthChangedEvent.AddUObject(this, &ThisClass::OnTargetActorHealthChanged);
		
		// UE_LOG(LogController, Log, TEXT("AAR_AIController::SetTargetActor, DelHandle_OnTargetActorHealthChanged ? %d"), DelHandle_OnTargetActorHealthChanged.IsValid());
	}

	BB->SetValueAsObject(NAME_TargetActor, ValidTarget);
	SetFocus(ValidTarget, EAIFocusPriority::Gameplay);

	StopMovement();   // Optional, according to needs

	// UE_LOG(LogGame, Log, TEXT("AAR_AIController::SetTargetActor -> %s"), *ValidTarget->GetActorNameOrLabel());
}

void AAR_AIController::OnTargetActorHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	// UE_LOG(LogController, Log, TEXT("AAR_AIController::OnTargetActorHealthChanged, Entered this member function"));
	
	if (FMath::IsNearlyZero(NewHealth))
	{
		// UE_LOG(LogController, Log, TEXT("AAR_AIController::OnTargetActorHealthChanged, FMath::IsNearlyZero(NewHealth) is true"));
		
		UBlackboardComponent* BB = GetBlackboardComponent();
		if (!BB) return;
		
		// Cancel Targeting, set to default
		BB->SetValueAsObject(NAME_TargetActor, LocalPlayerRef.Get());
		SetFocus(LocalPlayerRef.Get(), EAIFocusPriority::Gameplay);
		
		if (TargetActorASComp.Get())
		{
			FOnAttributeChanged& TargetHealthChangedEvent = 
				TargetActorASComp->GetAttributeListener(SharedGameplayTags::Attribute_Health);
			
			if (DelHandle_OnTargetActorHealthChanged.IsValid())	// if (TargetHealthChangedEvent.IsBoundToObject(this))
			{
				TargetHealthChangedEvent.Remove(DelHandle_OnTargetActorHealthChanged);
				DelHandle_OnTargetActorHealthChanged.Reset();
			}
		}
	}
	
	// ...
	
}
