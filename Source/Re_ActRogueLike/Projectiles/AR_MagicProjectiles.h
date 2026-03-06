// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_MagicProjectiles.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
UCLASS()
class RE_ACTROGUELIKE_API AAR_MagicProjectiles : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_MagicProjectiles();

protected:
	// Collision sphere for hit detection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;
	
	// Projectile movement logic
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;
	
	// Visual effect for projectile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;
	
	// Visual effect for projectile hit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* ImpactVFX;
	
	// I currently implement it in BP.
	/**
 	* Called when this actor's collision component hits another object.
 	*
 	* @param HitComponent     The component on this actor that was hit.
 	* @param OtherActor       The other actor involved in the collision.
 	* @param OtherComp        The component on the other actor that was hit.
 	* @param NormalImpulse    The impulse applied to resolve the collision.
 	* @param Hit              Detailed hit result data for the collision.
 	*/
	// UFUNCTION()
	// void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	// 	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	// Called when projectile died naturally
	virtual void LifeSpanExpired() override;
	
	// Called when utilities executing projectile explode 
	void Explode(const FHitResult* Hit = nullptr);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
