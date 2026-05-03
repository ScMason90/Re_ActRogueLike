// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AIController.h"

#include "AR_AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRoguelikeType.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"


// Sets default values
AAR_AIController::AAR_AIController()
{
	
}

// Called when the game starts or when spawned
void AAR_AIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ensureAlwaysMsgf(BehaviorTree, 
		TEXT("AAR_AIController::BeginPlay(), BehaviorTree is nullptr, please assign it")))return;
	
	RunBehaviorTree(BehaviorTree);
	
	FName TargetActor = FName("TargetActor"), MoveToLocation = FName("MoveToLocation");
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	check(PlayerPawn);
	
	// MoveToLocation variable maybe redundant for now.We currently use move to actor node to implement tracking down.
	GetBlackboardComponent()->SetValueAsVector(MoveToLocation, PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TargetActor, PlayerPawn);
}

void AAR_AIController::SetTargetActor(AActor* NewTarget)
{
	if (!IsValid(NewTarget))return;

	UBlackboardComponent* BB = GetBlackboardComponent();
	if (!BB) return;

	// Survival check
	if (UAR_ActionSystemComponent* ASComp = NewTarget->FindComponentByClass<UAR_ActionSystemComponent>())
	{
		if (ASComp && ASComp->IsDead()) return;
	}

	AActor* ValidTarget = NewTarget;
	if (NewTarget->IsPendingKillPending() || IsValid(NewTarget))
		ValidTarget = NewTarget->GetInstigator();

	BB->SetValueAsObject(NAME_TargetActor, ValidTarget);
	SetFocus(ValidTarget, EAIFocusPriority::Gameplay);

	StopMovement();   // Optional, according to needs

	UE_LOG(LogTemp, Warning, TEXT("AAR_AIController::SetTargetActor -> %s"), *NewTarget->GetActorNameOrLabel());
}
