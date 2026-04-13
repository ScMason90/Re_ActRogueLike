// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_EnvQueryContext_TargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Pawn.h"
#include "Re_ActRogueLike/Re_ActRoguelikeType.h"

void UAR_EnvQueryContext_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                     FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);	// Super::Parent logic is empty...
	
	// Expected Character
	APawn* QuerierPawn = Cast<APawn>(QueryInstance.Owner.Get());
	if (ensureMsgf(QuerierPawn, TEXT("UAR_EnvQueryContext_TargetActor::ProvideContext, QuerierPawn is nullptr!")))
	{
		AAIController* AIController = Cast<AAIController>(QuerierPawn->GetController());
		check(AIController);
		AActor* TargetActor = Cast<AActor>(
			AIController->GetBlackboardComponent()->GetValueAsObject(NAME_TargetActor));
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
	}
}
