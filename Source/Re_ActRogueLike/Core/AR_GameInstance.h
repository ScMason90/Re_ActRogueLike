// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AR_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<APawn>> AliveEnemies;
	
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld) override;
	
};
