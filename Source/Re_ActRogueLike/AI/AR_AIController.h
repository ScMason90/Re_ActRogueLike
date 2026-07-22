// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "AR_AIController.generated.h"

class UAR_ActionSystemComponent;
struct FGameplayTag;


/**
 *
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_AIController : public AAIController
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AAR_AIController();
	
	virtual void PreRegisterAllComponents() override;
	
protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	
	/** Set a new 'TargetActor' BlackBoard value
	 * @param NewTarget New Actor with a 'ASComp'  that should be a 'APawn' derived actor. */
	void SetTargetActorBB(AActor* NewTarget);

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UAIPerceptionComponent> PerceptionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	TWeakObjectPtr<APawn> LocalPlayerRef = nullptr;
	TWeakObjectPtr<UAR_ActionSystemComponent> TargetActorASComp = nullptr;
	
	FDelegateHandle DelHandle_OnTargetActorHealthChanged;
	
	void OnTargetActorHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth);
	
};
