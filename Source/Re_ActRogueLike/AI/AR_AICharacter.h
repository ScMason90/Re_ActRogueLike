// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_AICharacter.generated.h"

struct FGameplayTag;
class UAR_WorldUserWidget;
class UAR_ActionSystemComponent;
class UMaterialInstanceDynamic;

UCLASS()
class RE_ACTROGUELIKE_API AAR_AICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAR_AICharacter();
	
	virtual void PostInitializeComponents() override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;

protected:
	void OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth);
	
	void HandleDeath();
	
	void InitializeMIDs();
	
	void StartDissolve();
	void UpdateDissolve();
	FTimerHandle TimerHandle_Dissolve;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;

	/** @bug Can't trigger this delegate within build...Could be the problem of 
	 * subscription relationship within ue5 event system.Maybe you should try 'AI Perception' module in ue5?
	 * And watch some tutorial videos about it?
	 */
	// UFUNCTION()
	// void OnDeathBroadcasted(AActor* DeadActor);

protected:
	/* ------------- UI/UMG Widget Relative ---------------- */
	
	TObjectPtr<UAR_WorldUserWidget> ActiveHealthBar;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
	/* -------------Material Relative---------------- */
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToHitParamName;
	
	UPROPERTY(VisibleAnywhere, Category = "Material | Dissolve")
	float DissolveAmount;
	// Lower - faster, vice versa.
	UPROPERTY(EditAnywhere, Category = "Material | Dissolve")
	float DissolveLoopRate = 0.02f;
	// Lower - more detailed/smooth, vice versa.
	UPROPERTY(EditAnywhere, Category = "Material | Dissolve")
	float DissolveRate = 0.02f;
	
	UPROPERTY(VisibleAnywhere, Category = "Material | MaterialInstanceDynamic")
	TArray<UMaterialInstanceDynamic*> DynamicMIDs;
	
	/* -------------AI Pawn State---------------- */
	
	UPROPERTY(VisibleAnywhere, Category = "AI Pawn State")
	bool AIPawnDying = false;
	
	UPROPERTY(EditAnywhere, Category = "AI Pawn State")
	float DeathAnimDuration = 3.0f;
	
	/* -------------Effects---------------- */
	/* Anim&VFXs */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects | Anim&VFXs")
	TObjectPtr<UAnimMontage> DeathMontage;
};
