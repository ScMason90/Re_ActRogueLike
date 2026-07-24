// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PrimaryGameMode.h"

// Necessary compilation header files
#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"
#include "Re_ActRogueLike/AI/AR_AICharacter.h"
#include "Re_ActRogueLike/AI/AR_EnemyData.h"
#include "Re_ActRogueLike/Core/AR_GameInstance.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



static TAutoConsoleVariable<bool> CVarGameEnemySpawningEnabled(TEXT("game.EnemySpawningEnabled"), false, 
	TEXT("Disabling enemy spawning if debugging on purposes."), ECVF_Default);

static TAutoConsoleVariable<int32> CVarGameEnemyLimit(TEXT("game.EnemyLimit"), 5, 
	TEXT("Define the maximum number of alive enemies in the level with this 'GameModeClass' activating."),ECVF_Default);


AAR_PrimaryGameMode::AAR_PrimaryGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	
	PlayerStateClass = AAR_PlayerState::StaticClass();
	
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
		
		UE_LOG(LogGameMode, Log, TEXT("void AAR_PrimaryGameMode::StartPlay(), Init Seed: %d of Director: %s"), 
			Director.RandomStream_EnemySelection.GetInitialSeed(), *Director.DebugDisplayName);
		
		if (Director.EnemySpawnTable)
		{
			Director.EnemySpawnTable->GetAllRows<FEnemySpawnData>(
				"AAR_PrimaryGameMode::StartPlay, init a 'Director.CachedRows'", Director.CachedRows);
			
			/* '.Empty()'&'.Reserver(.Num())' the Director.PrefixWeights and D..r.CachedRows then ensure async calculate and allocate them again
			, if FAR_DirectorData supports dynamic (SpawnWeight/EnemyPool/WaveChanges) or activating GameMode::StartPlay() need to be called not only once*/
			
			// Rebuild after any of 'Director' that have an impact on 'D.PrefixWeights & D.CachedRows' change 
			// Director.PrefixWeights.Empty();
			// Director.PrefixWeights.Reserve(Director.PrefixWeights.Num());
			// Director.CachedRows.Empty();
			// Director.CachedRows.Reserve(Director.CachedRows.Num());
			
			Director.TotalSpawnWeight = 0.0f;
			for (FEnemySpawnData* Row : Director.CachedRows)
			{
				Director.TotalSpawnWeight += Row->SpawnWeight;
				Director.PrefixWeights.Add(Director.TotalSpawnWeight);
			}
		}
	}
	
#if 0
	// Make sure we have assigned at least one pickup class - legacy credit system (spawn coins)
	if (PickupClasses.Num() > 0)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
			this, PickupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance)) 
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AAR_PrimaryGameMode::OnPickupSpawnQueryFinished);
	}
#endif
	
}

void AAR_PrimaryGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	float TotalElapsedTime = GetWorld()->TimeSeconds;
	
	int32 KeyID = ONSCREENDEBUGKEY_SPAWNDIRECTOR;
	for (FAR_DirectorData& Director : Directors)
	{
		if (Director.EnemySpawnTable == nullptr)
		{
			UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::Tick, one Director.EnemySpawnTable is nullptr!"));
			continue;
		}
		
		float CreditPerSecond = Director.CreditGainCurve.GetRichCurveConst()->Eval(TotalElapsedTime);
		Director.CurrentCredits += CreditPerSecond * DeltaSeconds;
		
		FString DebugMsg = FString::Printf(TEXT("void AAR_PrimaryGameMode::Tick, Director:%s with KeyID:%d"
			"\nCurrentCredits:%f\tNextTickTime:%f\n"), *Director.DebugDisplayName, KeyID, Director.CurrentCredits, Director.NextTickTime);
		GEngine->AddOnScreenDebugMessage(KeyID, PrimaryActorTick.TickInterval, Director.DebugColor, DebugMsg);
		KeyID++;	// Actually Director.RandomStream_EnemySelection.GetInitialSeed() can be used as Debug 'KeyID'...that would be hard to recognize.
		
		if (Director.NextTickTime > TotalElapsedTime) continue;
		
		bool bSuccessSpawn = TrySpawnEnemy(Director);
		Director.NextTickTime = TotalElapsedTime + (bSuccessSpawn ? Director.TickInterval : Director.TimeBetweenWaves);
		
		// UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::Tick, one Director.CurrentCredits: %f"), Director.CurrentCredits);
	}
	
}

bool AAR_PrimaryGameMode::TrySpawnEnemy(FAR_DirectorData& Director)
{
	const int32 MaxEnemiesLimit = CVarGameEnemyLimit.GetValueOnGameThread();
	UAR_GameInstance* GI = GetGameInstance<UAR_GameInstance>();
	if (GI->AliveEnemies.Num() >= MaxEnemiesLimit)
	{
		UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::TrySpawnEnemy, Reached CVarGameEnemyLimit at %d"), MaxEnemiesLimit);
		return false;
	}
	
	// Director.EnemySpawnTable->GetAllRows("AAR_PrimaryGameMode::TrySpawnEnemy, Rebuild Director.CachedRows due to ...EnemySpawnTable changed", Director.CachedRows);
	
	float SelectedWeight = Director.RandomStream_EnemySelection.FRandRange(0.0f, Director.TotalSpawnWeight);
	
	// row 0 - 10 weight (10 total)
	// row 1 - 15 weight (25 total)
	// row 2 - 5 weight (30 total)
	// e.g. SelectedWeight (28) selects row 2, which ranges from 26-30 weight.
	
	// Result a index of binary search 'Director.PrefixWeights'. Corresponding a FEnemySpawnData row of 'Director.CachedRow'
	int32 SelectedIndex = Algo::LowerBound(Director.PrefixWeights, SelectedWeight);	
	FEnemySpawnData* SelectedRow = Director.CachedRows[SelectedIndex];
	
	UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::TrySpawnEnemy\tSelectedWeight = %s, SelectedIndex = %d"), 
		*FString::SanitizeFloat(SelectedWeight), SelectedIndex);

	if (Director.CurrentCredits < SelectedRow->SpawnCosts)
	{
		UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::TrySpawnEnemy, Lack of credits to spawn enemy using data asset; %s"), 
			*SelectedRow->EnemyData.GetAssetName());
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
	
	// UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::SpawnEnemyQueryCompleted, SpawnLocation = %s"), *SpawnLocation.ToString());
	
	SelectedEnemy->EnemyData.LoadAsync(FLoadSoftObjectPathAsyncDelegate::CreateUObject(this, 
		&ThisClass::OnEnemyDataLoaded, SpawnLocation, SelectedEnemy));
}

void AAR_PrimaryGameMode::OnEnemyDataLoaded(const FSoftObjectPath& LoadedObjectPath, UObject* LoadedObject,
	FVector SpawnLocation, FEnemySpawnData* SelectedEnemy)
{
#if !UE_BUILD_SHIPPING
	if (!CVarGameEnemySpawningEnabled.GetValueOnGameThread())
	{
		UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::OnEnemyDataLoaded, "
								"EnemySpawn disabled via CVarGameEnemySpawningEnabled as false!"));
		return;
	}
#endif
	
	FActorSpawnParameters EnemySpawnParams = FActorSpawnParameters();
	FTransform SpawnTM = FTransform(SpawnLocation);
	
	UAR_EnemyData* EnemyData = SelectedEnemy->EnemyData.Get();
	
	AAR_AICharacter* NewEnemy = GetWorld()->SpawnActorDeferred<AAR_AICharacter>(EnemyData->EnemyClass, FTransform::Identity);
	NewEnemy->SetEnemyData(EnemyData);
	
	// UE_LOG(LogGameMode, Log, TEXT("AAR_PrimaryGameMode::OnEnemyDataLoaded, NewEnemy = %s"), *GetNameSafe(NewEnemy));
	
	// Apply attributes override
	
	// Manually call to run construction script
	UGameplayStatics::FinishSpawningActor(NewEnemy, SpawnTM);
	
	UE_VLOG_SPHERE(this, LogGameMode, Log, SpawnLocation, 32.0f, FColor::Orange, 
		TEXT("AAR_PrimaryGameMode::OnEnemyDataLoaded,\nSelectedEnemy;  EnemyClass:%s | SpawnCosts:%.2f"),
		*GetNameSafe(EnemyData->EnemyClass), SelectedEnemy->SpawnCosts);
	
	// Add Buffs/Debuffs, etc.
	
	if (IsValid(NewEnemy))
	{
		UAR_ActionSystemComponent* ASComp = NewEnemy->GetASComp();
	
		for (TSubclassOf<UAR_Action> ActionClass : EnemyData->Actions)
		{
			ASComp->GrantAction(ActionClass);
		}
	}
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
		UAR_ActionSystemComponent* ASComp = ActorToKill->FindComponentByClass<UAR_ActionSystemComponent>();
		if (!ASComp)
		{
			SkippedCount++;
			continue;
		}
		if (!UAR_GameplayStatics::IsDying(ASComp))
		{
			KilledCount++;	// Class suicide
			UAR_GameplayStatics::Kill(this, ASComp);
		}
	}
	UE_LOG(LogGameMode, Warning, TEXT("AAR_PrimaryGameMode::KillAllOfClass(%s) => Killed: %d | Skipped: %d"),
		*ClassToKill->GetName(), KilledCount, SkippedCount);
}

// Player respawn... PlayerController
// Reward credits when player killed an enemy... GameInstance
