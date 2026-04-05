// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAR_AIController::AAR_AIController()
{
	
}

// Called when the game starts or when spawned
void AAR_AIController::BeginPlay()
{
	Super::BeginPlay();
	
	RunBehaviorTree(BehaviorTree);
	
	FName TargetActor = FName("TargetActor"), MoveToLocation = FName("MoveToLocation");
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	check(PlayerPawn);
	
	GetBlackboardComponent()->SetValueAsVector(MoveToLocation, PlayerPawn->GetActorLocation());
	GetBlackboardComponent()->SetValueAsObject(TargetActor, PlayerPawn);
}

