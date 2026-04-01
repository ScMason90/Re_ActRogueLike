// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_PickupActorBase.h"
#include "AR_HealthPotion.generated.h"

class UStaticMeshComponent;

UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_HealthPotion : public AAR_PickupActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_HealthPotion();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	float HealingAmount = 50.0f;
	
	virtual void OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
public:
	
};
