// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameModeBase.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/AI/AR_AICharacter.h"
#include "Re_ActRogueLike/Player/AR_PlayerController.h"

AAR_GameModeBase::AAR_GameModeBase()
{
	PlayerControllerClass = AAR_PlayerController::StaticClass();
	
	SpawnTimerInterval = 5.0f;
}

void AAR_GameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether it's allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this,
		&AAR_GameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void AAR_GameModeBase::SpawnBotTimerElapsed()
{
	float MaxBotCount = 5.0f;
	if (DifficultyCurve) MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->GetTimeSeconds());

	int32 NumOfAliveBots = 0;
	for (TActorIterator<AAR_AICharacter> It(GetWorld()); It; ++It)
	{
		AAR_AICharacter* Bot = *It;
		if (!Bot) continue;

		UAR_ActionSystemComponent* ASComp = Bot->FindComponentByClass<UAR_ActionSystemComponent>();
		if (ASComp && !ASComp->IsDead())
		{
			NumOfAliveBots++;
			if (NumOfAliveBots >= MaxBotCount) break;   
		}
	}

	if (NumOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, 
			TEXT("AAR_GameModeBase::OnQueryFinished, Bot count reached limit: %d / %.0f"), NumOfAliveBots, MaxBotCount);
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_GameModeBase::OnQueryFinished);
	}
}

void AAR_GameModeBase::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAR_GameModeBase::OnQueryFinished, Spawn bot EQS Query Failed!"));
		return;
	}
	
	TArray<FVector> QueryLocations;
	if (QueryInstance->GetQueryResultsAsLocations(QueryLocations) && QueryLocations.Num() > 0)
	{
		FVector SpawnLocation = QueryLocations[0];
		SpawnLocation += FVector(
			FMath::RandRange(-50.f, 50.f), 
			FMath::RandRange(-50.f, 50.f), 
			FMath::RandRange(100.0f, 150.0f));	// Random offset to avoid overlapping collision or stuck in floor

		if (MinionRangedClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			GetWorld()->SpawnActor<AActor>(MinionRangedClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			UE_LOG(LogTemp, Log, 
				TEXT("AAR_GameModeBase::OnQueryFinished, Spawned new bot at %s"), *SpawnLocation.ToString());
		}
	}
	else UE_LOG(LogTemp, Warning, TEXT("AAR_GameModeBase::OnQueryFinished, EQS returned no valid locations!"));
}
