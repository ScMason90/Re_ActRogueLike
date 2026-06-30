// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_ExplosiveBarrel.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;
class UAudioComponent;
class UStaticMeshComponent;
class URadialForceComponent;


/** May expand this class integrating 'DamageSystem' so that it can 'Radiating Damage' Actor in radial scope.
 *
 */
UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_ExplosiveBarrel : public AActor
{
	
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AAR_ExplosiveBarrel();
	
	/** Apply damage to this actor.
	 *	@param DamageAmount		How much damage to apply.
	 *	@param DamageEvent		Data package that fully describes the damage received.
	 *	@param EventInstigator	The Controller responsible for the damage.
	 *	@param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	 *	@return					The amount of damage actually applied.
	 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;
	
	// Bind hit event after components are initialized
	virtual void PostInitializeComponents() override;

protected:
	
	// Barrel mesh with physics simulation enabled
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	// Radial force used to simulate explosion impulse
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<URadialForceComponent> RadialForceComponent;
	
	UPROPERTY(EditAnywhere, Category = "Explosive Barrel")
	float ExplosionDelay = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive Barrel")
	TObjectPtr<UNiagaraSystem> BurningVFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive Barrel")
	TObjectPtr<USoundBase> BurningSFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive Barrel")
	TObjectPtr<UNiagaraSystem> ExplosionVFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive Barrel")
	TObjectPtr<USoundBase> ExplosionSFX;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveBurningVFXComp = nullptr;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveBurningSFXComp = nullptr;
	
	bool bExploded = false;
	
	FTimerHandle ExplosionTimerHandle;
	
	void Explode();
	
	// TODO: Considering make a explosive object base cpp class and implement 'DamageSystem' when explode after hit?
	// virtual void OnImpact(AActor* Instigator, const FHitResult& Hit);
	
};
