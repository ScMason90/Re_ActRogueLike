// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_PlayerCharacter.generated.h"

struct FGameplayTag;
struct FInputActionInstance;
struct FInputActionValue;
class UAR_ActionSystemComponent;
class UAnimMontage;
class UCameraComponent;
class UInputAction;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	
	/* ------------------- Components ---------------------*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	/* ------------------- Effect --------------------- */
	//				Preferences & FXs
	/* Anim */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | Anim")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Effect | Anim")
	float DeathMontageDuration = 1.0f;
	
	/* VFX */	
	
	
	
	/* ---------------------- Input Action ----------------------- */
	/* Movements */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Sprint;
	
	/* Combat and fight */
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireMagicProj;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireTeleportProj;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireBlackHole;
	
	/* ------------------- Material Relative ------------------- */
	
	// Encode indices of 'CustomPrimitiveData' for Overlay Material Instances?
	
	/* --------------------- Player State ---------------------- */
	
	/** Using GameplayTags to manage any 'PawnState' if needed. 
	 *  Usually we implement GameplayAbilitySystem modular of UE5 to achieve that.But not for our own 'GAS' now*/ 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player State")
	bool bPlayerDying = false; 

public:
	
	// Sets default values for this character's properties
	AAR_PlayerCharacter();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	
	/** Death logic (RAW). There is a bug when enter this execution that it might receive damage still... 
	 * Problem could be the collision set or untimely dead/lifespan ends. 
	 */
	void HandleDeath();
	
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionInstance& InValue);
	
	void OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth);
	
	FTimerHandle TimerHandle_Ragdoll;
	
	FTimerHandle TimerHandle_Overlay;

public:
	
	UFUNCTION()
	void StartAction(const FInputActionInstance& Instance, FGameplayTag InActionName);
	UFUNCTION()
	void StopAction(const FInputActionInstance& Instance, FGameplayTag InActionName);
	
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);
	
};
