// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/Core/AR_InteractionInterface.h"
#include "AR_PickupActorBase.generated.h"

class USphereComponent;

UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_PickupActorBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_PickupActorBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> OverlapComponent;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	// float RespawnTime;
	
	// FTimerHandle HideAndCooldownPickup_TimerHandle;
	
	// void SetPickupState(bool bNewIsActivate);
	
	// UFUNCTION()
	// void ShowPickup();
	
	// void HideAndCooldownPickup();
	
	UFUNCTION()
	virtual void OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	virtual void PostInitializeComponents() override;
};
