// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_TargetDummy.generated.h"


struct FGameplayTag;
class UAR_ActionSystemComponent;


/**
 * Used to test any new features that might be added to 'AAR_Pawn' class.
 */
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

public:
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void PostInitializeComponents() override;
	
protected:
	
	FDelegateHandle DelHandle_OnHealthChanged;
	
	void OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth);
	
	void HandleDeath() const;
	
	// Cpp delegate for MULTICAST_DYNAMIC FOnGameplayTagCountUpdated delegate of ASComp. Can also subscribe from BP.
	UFUNCTION()
	void OnGameplayTagCountUpdated(FGameplayTag UpdatedTag, int32 NewCount);
	
};
