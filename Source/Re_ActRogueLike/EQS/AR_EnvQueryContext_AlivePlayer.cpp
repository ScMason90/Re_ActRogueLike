// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_EnvQueryContext_AlivePlayer.h"

#include "EngineUtils.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerCharacter.h"

void UAR_EnvQueryContext_AlivePlayer::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                      FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	TArray<AActor*> AlivePlayers;
	
	for (AAR_PlayerCharacter* Player : TActorRange<AAR_PlayerCharacter>(QueryInstance.World))
	{
		if (UAR_GameplayStatics::IsActorAlive(Player)) AlivePlayers.Add(Player);
	}
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, AlivePlayers);
}
