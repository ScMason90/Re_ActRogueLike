// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_TargetDummy.generated.h"

struct FGameplayTag;
class UAR_ActionSystemComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_TargetDummy : public AActor
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AAR_TargetDummy();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dummy State")
	bool bDummyDying = false;
	
	void OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth);

public:
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;
	
};
