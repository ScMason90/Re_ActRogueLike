// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AR_PlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UAR_InteractionComponent;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Interact;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UAR_InteractionComponent> InteractionComponent;
	
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;
	
	void StartInteract();
	
public:
	
	AAR_PlayerController();
	
};
