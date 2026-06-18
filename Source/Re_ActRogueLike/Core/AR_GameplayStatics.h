// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AR_GameplayStatics.generated.h"

class UAR_ActionSystemComponent;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_GameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable)
	static bool IsFullHealth(UAR_ActionSystemComponent* ASComp);
	
	/**	Return FMath::IsNearlyZero(HealthValue) || HealthValue < 0.0f... 
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable)
	static bool IsDying(UAR_ActionSystemComponent* ASComp);
	
	// If game design allow health less than Zero to be valid, then you might need another functionality as checkpoint
	
};
