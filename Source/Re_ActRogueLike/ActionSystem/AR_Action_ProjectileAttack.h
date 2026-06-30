// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_Action.h"
#include "AR_Action_ProjectileAttack.generated.h"

class ACharacter;
class USoundBase;
class AAR_ProjectileBase;
class UNiagaraSystem;
class UAnimMontage;
class AActor;

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_Action_ProjectileAttack : public UAR_Action
{
	GENERATED_BODY()
	
	UAR_Action_ProjectileAttack();

public:
	
	virtual void StartAction_Implementation() override;
	
protected:
	
	/* --- Post spawn tweaking param for functionality - 'AdjustedProjSpawnTransform()' --- */
	
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack")
	FName MuzzleSocketName;
	
	UPROPERTY(EditAnywhere, Category = "ProjectileAttack")
	float LineTraceEndOffset;
	
	
	/* ----------- Projectile Sources ----------- */
	
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileSource")
	TSubclassOf<AAR_ProjectileBase> ProjectileClass;
	
	
	/* ------------------- Effect --------------------- */
	//				Preferences & FXs
	/* Anim */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | Anim")
	TObjectPtr<UAnimMontage> FireMontage;	// Casting Anim
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | Anim")
	float FireDelay = 0.0f;	// Casting Duration - Depend on how long 'FireMontage' takes
	
	/* VFX */	
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect | VFX")
	TObjectPtr<UNiagaraSystem> CastingVFX;	// NS played during cast animation
	
	/* SFX */
	
	UPROPERTY(EditDefaultsOnly, Category = "Effects | SFX")
	TObjectPtr<USoundBase> CastingSFX;
	
public:
	
	FTransform AdjustedProjSpawnTransform(ACharacter& Character, FName InSocketName, float InLineTraceEndOffset);
	
};
