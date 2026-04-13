// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "AR_EnvQueryContext_TargetActor.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_EnvQueryContext_TargetActor : public UEnvQueryContext
{
	GENERATED_BODY()
	
public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
};
