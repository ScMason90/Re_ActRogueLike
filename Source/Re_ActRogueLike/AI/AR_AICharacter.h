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
	
	void OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth);
	
	FDelegateHandle DelHandle_OnHealthChanged;
	
	/** Death logic (RAW). There is a bug when enter this execution that it might receive damage still... 
	 * Problem could be the collision set or untimely dead/lifespan ends. 
	 */
	void HandleDeath();
	
	void InitializeMIDs();
	
	void StartDissolve();
	void UpdateDissolve();
	FTimerHandle TimerHandle_Dissolve;
	
	FTimerHandle TimerHandle_Overlay;
	
	UFUNCTION()
	void OnGameplayTagCountUpdated(FGameplayTag UpdatedTag, int32 NewCount);
	
	FTimerHandle TimerHandle_Burning;

public:
	
	// Try adding 'AI Perception' ue5 module? It's a component
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;

protected:
	
	/* ------------- UI/UMG Widget Relative ---------------- */
	
	UPROPERTY()
	TObjectPtr<UAR_WorldUserWidget> ActiveHealthBar;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	
	/* ------------- Material Relative ---------------- */
	// Encode indices of 'CustomPrimitiveData' for Overlay Material Instances?
	
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
	
	/* ------------ AI Pawn State ----------- */
	
	UPROPERTY(VisibleAnywhere, Category = "AI Pawn State")
	bool bAIPawnDying = false;
	
	/* ------------- Effect -------------- */
	//		Preferences & FXs
	/* Anim */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | Anim")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Effect | Anim")
	float DeathAnimDuration = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | Anim")
	TObjectPtr<UAnimMontage> StunnedMontage;
	
	/* VFX */
	
	
	
};
