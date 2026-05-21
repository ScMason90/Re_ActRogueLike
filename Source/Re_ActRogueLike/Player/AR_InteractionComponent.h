// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_InteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RE_ACTROGUELIKE_API UAR_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAR_InteractionComponent();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRadius = 800.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float DistanceToWeightScale = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float DirectionWeightScale = 1.0;
	
	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void PrimaryInteraction();
};
