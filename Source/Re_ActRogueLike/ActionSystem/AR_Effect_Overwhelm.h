// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_Effect.h"
#include "AR_Effect_Overwhelm.generated.h"


/**	Utility 'EffectTrigger' class responsible for 'MinionStun'.
 * Tracks recent damage received and applies Stun debuff when a threshold is reached.
 * TODO: Create a new Specified 'StatusEffectManager/Trigger' GameplayTag category or 'AR_Effect_Manager/TriggerBase' class? 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_Effect_Overwhelm : public UAR_Effect
{
	GENERATED_BODY()
	
public:
	
	UAR_Effect_Overwhelm();
	
protected:
	
	// Amount of damage required (in one hit or summed) to apply the Debuff effect
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	float DamageThreshold = 35.0f;	
	
	// Damage within this timeframe is summed to potentially reach the 'DamageThreshold'
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	float TimeDeltaThreshold = 1.0f;
	
	// The debuff to apply when damage threshold is reached (e.g. Stun)
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	TSubclassOf<UAR_Action> EffectOnThreshold;
	
	float LastDamageTime = 0.0f;
	float SummedRecentDamage = 0.0f;
	
	FDelegateHandle DelegateHandle_Health;
	
	UFUNCTION()
	void OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth);
	
public:
	
	virtual void StartAction_Implementation() override;
	virtual void StopAction_Implementation() override;
	
};
