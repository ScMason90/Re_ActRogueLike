// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_AICharacter.generated.h"

class UAR_ActionSystemComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_AICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAR_AICharacter();
	
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth, float Delta);

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;

protected:
	/* -------------Material Relative---------------- */
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToHitParamName;
	
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToDeadParamName;
	
	/* -------------Player State---------------- */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character State")
	bool IsPlayerDead = false;
	
	/* -------------Effects---------------- */
	/* Anim&VFXs */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects | Anim&VFXs")
	TObjectPtr<UAnimMontage> DeathMontage;
};
