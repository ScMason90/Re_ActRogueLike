// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AR_EnemyData.generated.h"

class UAR_Action;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_EnemyData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APawn> EnemyClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAR_Action>> Actions;
 	
	// Example: Behavior Tree (BTComp) from AI Controller...
	
};
