// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PrimaryGameMode.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "Curves/CurveFloat.h"
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/AI/AR_AICharacter.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"
#include "Re_ActRogueLike/Player/AR_PlayerController.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"


static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("game.mode.spawnbots"), false, TEXT("Enable AI bot spawning. false = disabled.This only works before PIE in editor"), ECVF_Default);


AAR_PrimaryGameMode::AAR_PrimaryGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
	
	PlayerStateClass = AAR_PlayerState::StaticClass();
	
	SpawnTimerInterval = 5.0f;
	CreditsPerKill = 20;
	
	DesiredPickupCount = 10;
	RequiredPickupDistance = 1000;
	
}

void AAR_PrimaryGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (EnemySpawnTable == nullptr)
	{
		UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::Tick, EnemySpawnTable is nullptr!"));
		return;
	}
	
	TArray<FEnemySpawnData*> AllRows;	// All rows of 'EnemySpawnTable'
	EnemySpawnTable->GetAllRows("SelectedEnemy", AllRows);
	
	// UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::Tick, AllRows.Num() = %d"), AllRows.Num());
	
	FEnemySpawnData* SelectedEnemy = AllRows[FMath::RandRange(0, AllRows.Num() - 1)/*int32, SelectedIndex*/];
	
	FQueryFinishedSignature SpawnEnemyCompletedDelegate = 
		FQueryFinishedSignature::CreateUObject(this, &ThisClass::SpawnEnemyQueryCompleted, SelectedEnemy);
	
	FEnvQueryRequest SpawnEnemyRequest(SpawnEnemyLocationQuery, this);
	SpawnEnemyRequest.Execute(EEnvQueryRunMode::SingleResult, SpawnEnemyCompletedDelegate);
}

void AAR_PrimaryGameMode::SpawnEnemyQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult, FEnemySpawnData* SelectedEnemy)
{
	FVector SpawnLocation = QueryResult->GetItemAsLocation(0);
	SpawnLocation.Z += 100.0f;	// Land to the ground instead of stuck in it due to spawning failed. Randomize this?
	
	// UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::SpawnEnemyQueryCompleted, SpawnLocation = %s"), *SpawnLocation.ToString());
	
	SelectedEnemy->EnemyClass.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, 
		&ThisClass::OnEnemyClassLoaded, SpawnLocation, SelectedEnemy));
}

void AAR_PrimaryGameMode::OnEnemyClassLoaded(const FSoftObjectPath& LoadedObjectPath, UObject* LoadedObject,
	FVector SpawnLocation, FEnemySpawnData* SelectedEnemy)
{
	FActorSpawnParameters EnemySpawnParams = FActorSpawnParameters();
	
	AAR_AICharacter* NewEnemy = GetWorld()->SpawnActor<AAR_AICharacter>(
		SelectedEnemy->EnemyClass.Get(), SpawnLocation, FRotator::ZeroRotator, EnemySpawnParams);
	
	// UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::SpawnEnemyQueryCompleted, NewEnemy = %s"), *GetNameSafe(NewEnemy));
	
	// Set Attributes, add Buffs/Debuffs, etc. 
}

/** Now I can't execute this uproject's 'Debug' mode through Rider.
 * It continues to detect a warning in 'AR_AICharacter()' constructor while I try to spawn bots.
 * May need to delete these spawn bots logic and implemented somewhere else. */
void AAR_PrimaryGameMode::StartPlay()
{
	Super::StartPlay();

#if 0
	// Continuous timer to spawn in more bots.
	// Actual amount of bots and whether it's allowed to spawn determined by spawn logic later in the chain...
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnEnemy, this,
		&AAR_PrimaryGameMode::SpawnEnemyTimerElapsed, SpawnTimerInterval, true);
#endif
	
	
	// Make sure we have assigned at least one pickup class
	if (PickupClasses.Num() > 0)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
			this, PickupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance)) 
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_PrimaryGameMode::OnPickupSpawnQueryFinished);
	}
}

void AAR_PrimaryGameMode::SpawnEnemyTimerElapsed()
{
#if !UE_BUILD_SHIPPING
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::SpawnEnemyTimerElapsed(), Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return;
	}
#endif
	
#if 0
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
			TEXT("AAR_PrimaryGameMode::OnQueryFinished, Bot count reached limit: %d / %.0f"), NumOfAliveBots, MaxBotCount);
		return;
	}
	
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this, SpawnEnemyLocationQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_PrimaryGameMode::OnBotSpawnQueryFinished);
	}
#endif
	
}

void AAR_PrimaryGameMode::OnBotSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::OnBotSpawnQueryFinished, Spawn bot EQS Query Failed!"));
		return;
	}
	
#if 0
	TArray<FVector> QueryLocations;
	if (QueryInstance->GetQueryResultsAsLocations(QueryLocations) && QueryLocations.Num() > 0)
	{
		FVector SpawnLocation = QueryLocations[0];
		SpawnLocation += FVector(
			FMath::RandRange(-50.f, 50.f), 
			FMath::RandRange(-50.f, 50.f), 
			FMath::RandRange(100.0f, 150.0f));	// Random offset to avoid overlapping collision or stuck in floor

		if (EnemyClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			GetWorld()->SpawnActor<AActor>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			UE_LOG(LogGame, Log, 
				TEXT("AAR_PrimaryGameMode::OnBotSpawnQueryFinished, Spawned new bot at %s"), *SpawnLocation.ToString());
		}
		else UE_LOG(LogGame, Log, TEXT("AAR_PrimaryGameMode::OnBotSpawnQueryFinished, Spawned failed with nullptr 'MinionRangedClass'"));
	}
	else UE_LOG(LogGame, Warning, TEXT("AAR_PrimaryGameMode::OnBotSpawnQueryFinished, EQS returned no valid locations!"));
#endif
	
}

void AAR_PrimaryGameMode::OnPickupSpawnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
	EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::OnPickupSpawnQueryFinished, Spawn bot EQS Query Failed!"));
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

void AAR_PrimaryGameMode::KillAllOfClass(TSubclassOf<AActor> ClassToKill)
{
	UWorld* World = GetWorld();
	if (!World || !ClassToKill)
	{
		UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::KillAllOfClass, Invalid World or Class"));
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
	UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::KillAllOfClass(%s) => Killed: %d | Skipped: %d"),
		*ClassToKill->GetName(), KilledCount, SkippedCount);
}

void AAR_PrimaryGameMode::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();
		RestartPlayer(Controller);
	}
}

void AAR_PrimaryGameMode::OnActorKilled(AActor* VictimActor, AActor* Killer)
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
