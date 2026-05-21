// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AR_PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, AAR_PlayerState*, PlayerState, int32, NewCredits, int32, Delta);

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	int32 Credits;
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	int32 GetCredits() const;
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	void AddCredits(int32 Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool RemoveCredits(int32 Delta);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;
	
protected:
	
};
