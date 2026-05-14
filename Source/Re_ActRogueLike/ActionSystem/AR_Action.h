// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	FName ActionName = FName("PrimaryAttack");

public:
	UAR_ActionSystemComponent* GetOwningASComponent() const;

	virtual void StartAction();
	
	FName GetActionName() const {return ActionName;}
	
};
