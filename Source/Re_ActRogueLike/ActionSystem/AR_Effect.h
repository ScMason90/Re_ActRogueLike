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
	
	/* Interval between each 'Tick' to apply effects. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	float Period = 0.0f;
	
	/** Adding the same Effect multiple times instead increases the stack size. Can be used to amplify certain effects. 
	 * Resets effect duration unless bResetDurationOnStackIncrease is false. */
	UPROPERTY(Transient, BlueprintReadOnly, Category = Effect)
	int32 StackCount = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect)
	bool bResetDurationOnStackIncrease = true;
	
	FTimerHandle TimerHandle_Duration;
	FTimerHandle TimerHandle_Period;


	/** Override in derived class.Nothing did in 'Super::...' base implement */
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodicEffect();
	
	void ResetDuration();
	
public:
	
	void IncrementStackSize();
	
	virtual void StartAction_Implementation() override;
	
	virtual void StopAction_Implementation() override;
	
};
