// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_BlackHoleProjectile.generated.h"

class URadialForceComponent;
UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_BlackHoleProjectile : public AAR_ProjectileBase
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<URadialForceComponent> RadialForceComponent;
	
	virtual void OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	// Override OnProjHit intend to shut down its logic
	virtual void OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
public:
	
	// Sets default values for this actor's properties
	AAR_BlackHoleProjectile();
	
	virtual void PostInitializeComponents() override;
	
	// Override PlayExplosionFXs_Implementation() intend to shut down its logic
	virtual void PlayExplosionFXs_Implementation(const FHitResult& Hit) override;
	
};
