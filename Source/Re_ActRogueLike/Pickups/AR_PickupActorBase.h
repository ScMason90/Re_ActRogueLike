// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/Core/AR_GameplayInterface.h"
#include "AR_PickupActorBase.generated.h"

class USphereComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_PickupActorBase : public AActor, public IAR_GameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_PickupActorBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	float RespawnTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;
	
	void SetPickupState(bool bNewIsActivate);
	
	UFUNCTION()
	void ShowPickup();
	
	void HideAndCooldownPickup();

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;	
};
