// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_PickupActorBase.h"
#include "AR_HealthPotion.generated.h"

class UStaticMeshComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_HealthPotion : public AAR_PickupActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_HealthPotion();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;
	
public:
	void Interact_Implementation(APawn* InstigatorPawn) override;
};
