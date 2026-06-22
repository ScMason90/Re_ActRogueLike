// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AR_GameplayStatics.generated.h"

class UAR_ActionSystemComponent;

/** 
 * Even so, the specific logic of all the utility functions related to ActionSystem still depends on your game design
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_GameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsFullHealth(UAR_ActionSystemComponent* ASComp);
	
	/**	Return FMath::IsNearlyZero(HealthValue)...Uncomment '|| HealthValue < 0.0f' if you want. 
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsDying(UAR_ActionSystemComponent* ASComp);
	
	// If game design allow health less than Zero to be valid, then you might need another functionality as checkpoint
	
	/**
	 * @return false if ActorToCheck didn't have a 'ASComp' or its 'ASComp's HealthValue > 0.0f.Also false if it's invalid actor.
	 * @param ActorToCheck A C++ pointer.Assign to the specific actor that you want to check if it's alive. 
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsActorAlive(AActor* ActorToCheck);
	
};
