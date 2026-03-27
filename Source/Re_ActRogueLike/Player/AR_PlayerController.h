// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AR_PlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
public:
	AAR_PlayerController();
};
