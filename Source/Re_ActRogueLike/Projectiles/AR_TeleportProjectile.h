// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_MagicProjectiles.h"
#include "AR_TeleportProjectile.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_TeleportProjectile : public AAR_MagicProjectiles
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_TeleportProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void ExplodeAndTeleport();
	
	FTimerHandle TimerHandle_Explode;
	FTimerHandle TimerHandle_Teleport;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
