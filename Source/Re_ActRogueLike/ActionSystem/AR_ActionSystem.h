// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AR_ActionSystem.generated.h"

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_ActionSystem : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	FName ActionName = FName("PrimaryAttack");

public:
	void StartAction();
	
	FName GetActionName() const {return ActionName;}
	
};
