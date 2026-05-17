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
	/* -------------Effects---------------- */
	/* Anim&VFXs */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects | FireProjectile | Anim&VFXs")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Effects | FireProjectile | Anim&VFXs")
	float DeathMontageDuration = 1.0f;
	
	FTimerHandle TimerHandle_Ragdoll;
	
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
	
	/* -------------Material Relative---------------- */
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToHitParamName;
	
	/* -------------Player State---------------- */
	// I think this will be replaced in future with GameplayTags in our 'GAS'.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player State")
	bool IsPlayerDead = false;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionInstance& InValue);
	
	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth, float Delta);

public:
	UFUNCTION()
	void StartAction(const FInputActionInstance& Instance, FGameplayTag InActionName);
	UFUNCTION()
	void StopAction(const FInputActionInstance& Instance, FGameplayTag InActionName);
	
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);
};
