// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_CoinTestActor.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_CoinTestActor : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AAR_CoinTestActor();

protected:

	UPROPERTY(VisibleAnywhere, Category = Components)
	TObjectPtr<USceneComponent> DefaultSceneComponent;

public:
	
	UFUNCTION(BlueprintCallable)
	void SpawnCoins(int32 SpawnCount);
	
};
