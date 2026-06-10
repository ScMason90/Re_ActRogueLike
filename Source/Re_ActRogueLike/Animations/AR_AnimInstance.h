// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AR_AnimInstance.generated.h"

struct FGameplayTag;
class UAR_ActionSystemComponent;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeBeginPlay() override;
	
protected:
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = StatusEffects)
	bool bIsSprinting;
	
	UPROPERTY(Transient, BlueprintReadOnly, Category = StatusEffects)
	bool bIsStunned;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAR_ActionSystemComponent> ASComp;	// ActionSystemComponent
	
	UFUNCTION()
	void OnTagUpdated(FGameplayTag UpdatedTag, int32 NewCount);
	
};
