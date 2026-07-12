// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_AICharacter.generated.h"


class UAR_EnemyData;
struct FGameplayTag;
class UAR_WorldUserWidget;
class UAR_ActionSystemComponent;
class UMaterialInstanceDynamic;


/** Run in DebugMode with Rider.Then any operation to instances BP asset derived from this class will get warning of constructing variables in AAR_AICharacter::AAR_AICharacter(). - 2026.6.23 
 * This case is still ambiguous and unclear. It happens from time to time.However I didn't trigger this today. It's an 'unstable' bug? - 2026.6.27 20:02
 * It won't trigger at all now. I think it might be because I cleared the redundant header files including the fixes? - 2026.7.7 16:29
 */
UCLASS(Abstract)
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
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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
	
	// Get the ActionSystemComponent of an AR_AICharacter class derived instance
	UAR_ActionSystemComponent* GetASComp() const {return ActionSystemComponent;}
	
	UAR_EnemyData* GetEnemyData() const {return EnemyData;}
	
	void SetEnemyData(UAR_EnemyData* NewEnemyData)
	{
		check(NewEnemyData);
		EnemyData = NewEnemyData;
	}

protected:
	
	// Try adding 'AI Perception' ue5 module? It's a component
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	UPROPERTY(Transient)
	TObjectPtr<UAR_EnemyData> EnemyData;
	
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
