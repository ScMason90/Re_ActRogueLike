// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Misc/App.h"
#include "UObject/SoftObjectPtr.h"
#include "AR_DeveloperSettings.generated.h"

class USoundBase;
class FApp;
class UStaticMesh;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig)
class RE_ACTROGUELIKE_API UAR_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(Config, EditDefaultsOnly, Category = Pickups)
	TSoftObjectPtr<UStaticMesh> CoinPickupMesh;
	
	UPROPERTY(Config, EditDefaultsOnly, Category = Pickups)
	TSoftObjectPtr<USoundBase> CoinPickupSound;
	
	UPROPERTY(Config, EditDefaultsOnly, Category = Pickups)
	FName CoinPickupTriggerParameter;
	
	virtual FName GetCategoryName() const override
	{
		return FApp::GetProjectName();
	}
	
};
