// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_ExplosiveBarrel.generated.h"


class USphereComponent;
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
	
	// Bind hit event after components are initialized
	virtual void PostInitializeComponents() override;
	
	/** Apply damage to this actor.
	 *	@param DamageAmount		How much damage to apply.
	 *	@param DamageEvent		Data package that fully describes the damage received.
	 *	@param EventInstigator	The Controller responsible for the damage.
	 *	@param DamageCauser		The Actor that directly caused the damage (e.g. the projectile that exploded, the rock that landed on you)
	 *	@return					The amount of damage actually applied.
	 */
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void LifeSpanExpired() override;

protected:
	
	// Barrel mesh with physics simulation enabled
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	// Radial force used to simulate explosion impulse
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<URadialForceComponent> RadialForceComponent;
	
	// Sphere collision to overlap actors and damage them
	// UPROPERTY(VisibleAnywhere, Category = "Components")
	// TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(EditAnywhere, Category = "Explosive Barrel")
	float ExplosionDelay = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | FXs")
	TObjectPtr<UNiagaraSystem> BurningVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | FXs")
	TObjectPtr<USoundBase> BurningSFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | FXs")
	TObjectPtr<UNiagaraSystem> ExplosionVFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | FXs")
	TObjectPtr<USoundBase> ExplosionSFX;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveBurningVFXComp = nullptr;
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveBurningSFXComp = nullptr;
	
	bool bExploded = false;
	
	FTimerHandle ExplosionTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Self-Impulse")
	float ImpulseSelfStrength = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Self-Impulse")
	float ImpulseSelfUpScalar = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Self-Impulse")
	float ImpulseSelfSideScalar = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Self-Impulse")
	FRandomStream RandomStream_ImpulseSelf;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Damage")
	TSubclassOf<UDamageType> DmgTypeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Explosive Barrel | Damage")
	float ExplosionDamageAmount = 70.0f;
	
	// TArray<AActor*> ActorsOverlappedSphere;	// Temporarily not using
	
	void Explode();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// TODO: Create a base explosive object class integrating 'DamageSystem'? You can refer AR_ProjectileBase class
	
	
	// Temporarily not using. Uncomment 'ApplyDamage' Relative SphereComponent if you want implement with collision collide based solution
	// 
	// UFUNCTION()
	// void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	// 	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	// 	
	// UFUNCTION()
	// void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
};
