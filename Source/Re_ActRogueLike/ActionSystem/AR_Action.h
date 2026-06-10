// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AR_Action.generated.h"

class UAR_ActionSystemComponent;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class RE_ACTROGUELIKE_API UAR_Action : public UObject
{
	GENERATED_BODY()

protected:
	
	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	FGameplayTag ActionName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	FGameplayTagContainer GrantTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	FGameplayTagContainer BlockedTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	float CooldownTime = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TMap<FGameplayTag, float> ActivationCost;

public:
	
	const FGameplayTagContainer& GetBlockedTags() const {return BlockedTags;}
	
	/** Since we load 'UAR_AttributeSet' and 'UAR_Action' or their derived classes only in 'UAR_ActionSystemComponent'
	 * , No need of any error check and validation.Keep that in mind we always restore database and
	 * execute relative application in 'ASComp' */
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	UAR_ActionSystemComponent* GetOwningASComponent() const;
	
	FGameplayTag GetActionName() const {return ActionName;}
	
	virtual bool ImplementsGetWorld() const override {return true;}

	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void StartAction();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Actions")
	void StopAction();
	
	bool CanStart() const;
	bool IsRunning() const {return bIsRunning;}
	float GetCooldownTimeRemaining() const;
	
protected:
	
	/* GameTime until the Action is available again */	
	UPROPERTY(Transient)
	float CooldownThreshold = 0.0f;
	
	UPROPERTY(Transient)
	bool bIsRunning = false;
	
};
