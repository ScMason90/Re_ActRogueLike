// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameModeBase.h"

#include "AR_GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/AI/AR_AICharacter.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"
#include "Re_ActRogueLike/Player/AR_PlayerController.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("game.mode.spawnbots"), true, TEXT("Enable AI bot spawning. false = disabled.This only works before PIE in editor"), ECVF_Default);

AAR_GameModeBase::AAR_GameModeBase()
{
	PlayerControllerClass = AAR_PlayerController::StaticClass();
	PlayerStateClass = AAR_PlayerState::StaticClass();
	
	SpawnTimerInterval = 5.0f;
	CreditsPerKill = 20;
	
	DesiredPickupCount = 10;
	RequiredPickupDistance = 1000;
	
}

void AAR_GameModeBase::StartPlay()
{
	Super::StartPlay();

	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether it's allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this,
		&AAR_GameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
	
	// Make sure we have assigned at least one pickup class
	if (PickupClasses.Num() > 0)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
			this, PickupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance)) 
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_GameModeBase::OnPickupSpawnQueryFinished);
	}
}

void AAR_GameModeBase::SpawnBotTimerElapsed()
{
#if !UE_BUILD_SHIPPING
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogGame, Warning, TEXT("AAR_GameModeBase::SpawnBotTimerElapsed(), Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
#endif
	
	float MaxBotCount = 5.0f;
	if (DifficultyCurve) MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->GetTimeSeconds());

	int32 NumOfAliveBots = 0;
	for (const AAR_AICharacter* Bot : TActorRange<AAR_AICharacter>(GetWorld()))
	{
		if (!Bot) continue;

		UAR_ActionSystemComponent* ASComp = Bot->FindComponentByClass<UAR_ActionSystemComponent>();
		if (ASComp && !UAR_GameplayStatics::IsDying(ASComp))
		{
			NumOfAliveBots++;
			if (NumOfAliveBots >= MaxBotCount) break;   
		}
	}

	if (NumOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogGame, Log, 
			TEXT("AAR_GameModeBase::OnQueryFinished, Bot count reached limit: %d / %.0f"), NumOfAliveBots, MaxBotCount);
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_GameModeBase::OnBotSpawnQueryFinished);
	}
}

void AAR_GameModeBase::OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_GameModeBase::OnBotSpawnQueryFinished, Spawn bot EQS Query Failed!"));
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
			UE_LOG(LogGame, Log, 
				TEXT("AAR_GameModeBase::OnBotSpawnQueryFinished, Spawned new bot at %s"), *SpawnLocation.ToString());
		}
	}
	else UE_LOG(LogGame, Warning, TEXT("AAR_GameModeBase::OnBotSpawnQueryFinished, EQS returned no valid locations!"));
}

void AAR_GameModeBase::OnPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_GameModeBase::OnPickupSpawnQueryFinished, Spawn bot EQS Query Failed!"));
		return;
	}
	
	TArray<FVector> QueryResultLocs = QueryInstance->GetResultsAsLocations();
	TArray<FVector> UsedLocs;	// Keep used locations to easily check distance between points
	int32 CurSpawnAmount = 0;

	// Break out if we reached the desired count or if we have no more potential positions remaining
	while (CurSpawnAmount < DesiredPickupCount && QueryResultLocs.Num() > 0)
	{
		// Pick a random location from remaining points.
		int32 RandomQueryLocIndex = FMath::RandRange(0, QueryResultLocs.Num() - 1);
		FVector QueryLocChosen = QueryResultLocs[RandomQueryLocIndex];
		
		// Remove to avoid picking again
		QueryResultLocs.RemoveAt(RandomQueryLocIndex);
		
		// Check minimum distance requirement
		bool bValidDistance = true;
		for (FVector OtherVector : UsedLocs)
		{
			if (const float DistanceTo = (QueryLocChosen - OtherVector).Size(); DistanceTo < RequiredPickupDistance)
			{
				// Show skipped locations due to distance
				// //DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);
				
				// too close, skip to next attempt
				bValidDistance = false;
				break;
			}
		}
		// Failed the distance test
		if (!bValidDistance) continue;
		
		// Keep for distance checks
		UsedLocs.Add(QueryLocChosen);
		CurSpawnAmount += 1;
		
		// Pick a random pickup-class
		TSubclassOf<AActor> PickupToSpawn = PickupClasses[FMath::RandRange(0, PickupClasses.Num() - 1)];
		QueryLocChosen += FVector(FMath::RandRange(-50.f, 50.f), // Random offset to randomize. 
			FMath::RandRange(-50.f, 50.f), FMath::RandRange(15.0f, 50.0f));	
		GetWorld()->SpawnActor<AActor>(PickupToSpawn, QueryLocChosen, FRotator::ZeroRotator);
	}
}

void AAR_GameModeBase::KillAllOfClass(TSubclassOf<AActor> ClassToKill)
{
	UWorld* World = GetWorld();
	if (!World || !ClassToKill)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_GameModeBase::KillAllOfClass, Invalid World or Class"));
		return;
	}
	
	int32 KilledCount = 0, SkippedCount = 0;
	for (TActorIterator<AActor> It(World, ClassToKill); It; ++It)
	{
		AActor* ActorToKill = *It;
		if (!IsValid(ActorToKill))
		{
			SkippedCount++;
			continue;
		}
		UAR_ActionSystemComponent* ASComp = UAR_ActionSystemComponent::GetASComp(ActorToKill);
		if (!ASComp)
		{
			SkippedCount++;
			continue;
		}
		if (!UAR_GameplayStatics::IsDying(ASComp))
		{
			KilledCount++;
			ASComp->Kill(ActorToKill/*'Class Suicide'*/);
		}
	}
	UE_LOG(LogGameMode, Warning, TEXT("AAR_GameModeBase::KillAllOfClass(%s) => Killed: %d | Skipped: %d"),
		*ClassToKill->GetName(), KilledCount, SkippedCount);
}

void AAR_GameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void AAR_GameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	// Deprecated...
	
	// Respawn Player after delay
	AAR_PlayerCharacter* Player = Cast<AAR_PlayerCharacter>(VictimActor);
	if (Player)
	{
		FTimerHandle TimerHandle_RespawnDelay;
		
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());
		
		float RespawnDelay = 5.0f;	// This depends on how long it will take for player character executing death logic and appearance
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
		
		return;
	}
	
	// Give Credits for Kill
	AAR_PlayerState* PS = nullptr;
	Player = Cast<AAR_PlayerCharacter>(Killer);
	// Do not 'UE_LOG' out here or access 'ActorName(Safe)OrLabel' here.It's invalid
	if (Player) PS = Cast<AAR_PlayerState>(Player->GetPlayerState());
	if (PS) PS->AddCredits(CreditsPerKill);
}
