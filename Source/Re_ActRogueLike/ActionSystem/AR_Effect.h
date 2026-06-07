// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_Action.h"
#include "AR_Effect.generated.h"

struct FTimerHandle;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_Effect : public UAR_Action
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	float Duration = 0.0f;
	
	FTimerHandle TimerHandle_Duration;
	
public:
	
	virtual void StartAction_Implementation() override;
	
	virtual void StopAction_Implementation() override;
	
};
