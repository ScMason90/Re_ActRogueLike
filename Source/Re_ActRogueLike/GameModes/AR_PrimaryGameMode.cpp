// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PrimaryGameMode.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/AI/AR_AICharacter.h"
#include "Re_ActRogueLike/Core/AR_GameInstance.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"
#include "Re_ActRogueLike/Player/AR_PlayerController.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"
#include "VisualLogger/VisualLogger.h"


static TAutoConsoleVariable<bool> CVarSpawnEnemy(TEXT("game.mode.spawn enemy"), true, 
	TEXT("Enable Enemy Spawning. false = disabled.Set before PIE in editor"), ECVF_Default);


AAR_PrimaryGameMode::AAR_PrimaryGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	
	PlayerStateClass = AAR_PlayerState::StaticClass();
	
	CreditsPerKill = 20;
	
	DesiredPickupCount = 10;
	RequiredPickupDistance = 1000;
	
}

void AAR_PrimaryGameMode::StartPlay()
{
	Super::StartPlay();
	
	FRandomStream GlobalRandomStream = FRandomStream(GlobalStartingSeed);
	
	for (FAR_DirectorData& Director : Directors)
	{
		int32 NewSeed = GlobalRandomStream.RandRange(0, MAX_int32 - 1);
		Director.RandomStream_EnemySelection = FRandomStream(NewSeed);
		
		UE_LOG(LogGameMode, Log, TEXT("void AAR_PrimaryGameMode::StartPlay(), "
			"Seed of one Director.RandomStream_EnemySelection = %d"), Director.RandomStream_EnemySelection.GetInitialSeed());
	}
	
	// Make sure we have assigned at least one pickup class - legacy credit system (spawn coins)
	if (PickupClasses.Num() > 0)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
			this, PickupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance)) 
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_PrimaryGameMode::OnPickupSpawnQueryFinished);
	}
}

void AAR_PrimaryGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	float TotalElapsedTime = GetWorld()->TimeSeconds;
	
	UAR_GameInstance* GI = GetGameInstance<UAR_GameInstance>();
	int32 MaxBotLimit = 5;	// This is hard-coded for 'SpawnEnemy(MinionRanged)'. didn't expose to Editor
	if (GI->AliveEnemies.Num() >= MaxBotLimit)
	{
		UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::Tick, Reached bot spawn limit at %d"), MaxBotLimit);
		return;
	}
	
	int32 KeyID = ONSCREENDEBUGKEY_SPAWNDIRECTOR;
	for (FAR_DirectorData& Director : Directors)
	{
		
#if !UE_BUILD_SHIPPING
		if (!CVarSpawnEnemy.GetValueOnGameThread())
		{
			UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::Tick, EnemySpawn disabled via CVarSpawnEnemy as false!"));
			return;
		}
#endif
		
		if (Director.EnemySpawnTable == nullptr)
		{
			UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::Tick, one Director.EnemySpawnTable is nullptr!"));
			return;
		}
		
		float CreditPerSecond = Director.CreditGainCurve.GetRichCurveConst()->Eval(TotalElapsedTime);
		Director.CurrentCredits += CreditPerSecond * DeltaSeconds;
		
		FString DebugMsg = FString::Printf(TEXT("void AAR_PrimaryGameMode::Tick, Director with 'KeyID' %d"
			"\nCurrentCredits:%f\tNextTickTime:%f"), KeyID, Director.CurrentCredits, Director.NextTickTime);
		GEngine->AddOnScreenDebugMessage(KeyID, PrimaryActorTick.TickInterval, FColor::Orange, DebugMsg);
		KeyID++;
		
		if (Director.NextTickTime > TotalElapsedTime) continue;
		
		bool bSuccessSpawn = TrySpawnEnemy(Director);
		Director.NextTickTime = TotalElapsedTime + (bSuccessSpawn ? Director.TickInterval : Director.TimeBetweenWaves);
		
		// UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::Tick, one Director.CurrentCredits: %f"), Director.CurrentCredits);
	}
	
}

bool AAR_PrimaryGameMode::TrySpawnEnemy(FAR_DirectorData& Director)
{
	TArray<FEnemySpawnData*> AllRows;	// All rows of 'EnemySpawnTable'
	Director.EnemySpawnTable->GetAllRows(
		"AAR_PrimaryGameMode::TrySpawnEnemy, SelectedEnemy in one Director.EnemySpawnTable", AllRows);
	
	// UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::TrySpawnEnemy, AllRows.Num() = %d"), AllRows.Num());
	
	// int32 SelectedIndex = Director.RandomStream_EnemySelection.RandRange(0, AllRows.Num()-1);	//FMath::RandRange(...)
	// FEnemySpawnData* SelectedRow = AllRows[SelectedIndex];
	
	float TotalWeights = 0.0f;
	for (FEnemySpawnData* Row : AllRows)
	{
		TotalWeights += Row->SpawnWeight;
	}
	float SelectedWeight = Director.RandomStream_EnemySelection.FRandRange(0.0f, TotalWeights);
	
	// row 0 - 10 weight (10 total)
	// row 1 - 15 weight (25 total)
	// row 2 - 5 weight (30 total)
	// e.g. SelectedWeight (28) selects row 2, which ranges from 26-30 weight.
	
	FEnemySpawnData* SelectedRow = nullptr;
	TotalWeights = 0.0f;
	for (FEnemySpawnData* Row : AllRows)
	{
		TotalWeights += Row->SpawnWeight;
		if (SelectedWeight <= TotalWeights)
		{
			SelectedRow = Row;
			break;
		}
	}

	if (Director.CurrentCredits < SelectedRow->SpawnCosts)
	{
		UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::TrySpawnEnemy, Not enough credits to spawn enemy %s"), 
			*SelectedRow->EnemyClass.GetAssetName());
		return false;
	}
	Director.CurrentCredits -= SelectedRow->SpawnCosts;
	
	FQueryFinishedSignature SpawnEnemyCompletedDelegate = 
		FQueryFinishedSignature::CreateUObject(this, &ThisClass::SpawnEnemyQueryCompleted, SelectedRow);
	
	FEnvQueryRequest SpawnEnemyRequest(Director.SpawnEnemyLocationQuery, this);
	int32 QueryID = SpawnEnemyRequest.Execute(EEnvQueryRunMode::SingleResult, SpawnEnemyCompletedDelegate);
	
	// EQS executed successfully
	return QueryID != INDEX_NONE;
}

void AAR_PrimaryGameMode::SpawnEnemyQueryCompleted(TSharedPtr<FEnvQueryResult> QueryResult, FEnemySpawnData* SelectedEnemy)
{
	FVector SpawnLocation = QueryResult->GetItemAsLocation(0);
	SpawnLocation.Z += 100.0f;	// Land to the ground instead of stuck in it due to spawning failed. Randomize this?
	
	// UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::SpawnEnemyQueryCompleted, SpawnLocation = %s"), *SpawnLocation.ToString());
	
	SelectedEnemy->EnemyClass.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, 
		&ThisClass::OnEnemyClassLoaded, SpawnLocation, SelectedEnemy));
}

void AAR_PrimaryGameMode::OnEnemyClassLoaded(const FSoftObjectPath& LoadedObjectPath, UObject* LoadedObject,
	FVector SpawnLocation, FEnemySpawnData* SelectedEnemy)
{
	FActorSpawnParameters EnemySpawnParams = FActorSpawnParameters();
	
	AAR_AICharacter* NewEnemy = GetWorld()->SpawnActor<AAR_AICharacter>(
		SelectedEnemy->EnemyClass.Get(), SpawnLocation, FRotator::ZeroRotator, EnemySpawnParams);
	
	// UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::SpawnEnemyQueryCompleted, NewEnemy = %s"), *GetNameSafe(NewEnemy));
	
	UE_VLOG_SPHERE(this, LogGameMode, Log, SpawnLocation, 32.0f, FColor::Orange, 
		TEXT("void AAR_PrimaryGameMode::SpawnEnemyQueryCompleted,\nFEnemySpawnData* SelectedEnemy;  SrcClass:%s | SpawnCosts:%s"),
		*GetNameSafe(SelectedEnemy->EnemyClass.Get()), *FString::SanitizeFloat(SelectedEnemy->SpawnCosts));
	
	// Set Attributes, add Buffs/Debuffs, etc. 
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
