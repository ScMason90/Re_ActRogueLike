// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AR_AIController.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_AIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_AIController();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

public:
	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	void SetTargetActor(AActor* NewTarget);
	
	UFUNCTION()
	void OnAllyOrTargetDied(AActor* DeadActor);
	
	// Redundant || Deprecated
	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	AActor* GetTargetActor() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};
