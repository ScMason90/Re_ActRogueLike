// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Components/AR_InteractionComponent.h"
#include "GameFramework/Character.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"
#include "AR_PlayerCharacter.generated.h"

class UParticleSystem;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> MagicProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> BlackHoleProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> TeleportProjectileClass;
	
	// TODO: May considering using separate 'anim montage & casting effect & spawn socket' for each proj action
	UPROPERTY(EditAnywhere, Category = "FireProjectile | Anim&Effect")
	UAnimMontage* SharedFireMontage;
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile | Anim&Effect")// Particle System played during attack animation
	UParticleSystem* SharedCastEffect;
	
	FTimerHandle TimerHandle_FireProj;
	
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToHitParamName;

public:
	// Sets default values for this character's properties
	AAR_PlayerCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere)
	UAR_InteractionComponent* InteractionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAR_AttributeComponent* AttributeComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void PrimaryInteract();
	
	// TODO: May considering introduce different AnimMontage for each proj attack?
	void FireProj(
		TSubclassOf<AActor> ProjClassToSpawn, UAnimMontage* AnimMontageToPlay, float TimeBeforProj,
		UParticleSystem* EffectWhenCast,
		/* And there are params passing to internal func-AdjustedProjSpawnTransform */
		FName InSocketName, float LineTraceEndOffset);
	
	void FireMagicProj();
	void FireBlackHole();
	void FireTeleportProj();
	
	FTransform AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset);
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor, UAR_AttributeComponent* OwningComp, float NewHealth, float Delta);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
