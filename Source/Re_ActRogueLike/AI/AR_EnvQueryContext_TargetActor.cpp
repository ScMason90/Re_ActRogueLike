// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_EnvQueryContext_TargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"

void UAR_EnvQueryContext_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* OwnerObj = QueryInstance.Owner.Get(); 
	if (!OwnerObj) return;
	
	// Expected Character - QuerierPawn
	const AActor* OwnerActor = Cast<AActor>(OwnerObj); 
	ensureMsgf(OwnerActor, TEXT("UAR_EnvQueryContext_TargetActor::ProvideContext, OwnerActor is nullptr!"));
	if (!OwnerActor)return;
	
	AAIController* AIController = Cast<AAIController>(OwnerActor->GetInstigatorController());
	check(AIController);

	const UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	check(BB);
	
	const AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!TargetActor) return;
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
}
