// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_MagicProjectile.generated.h"


class UAR_Effect;
class UDamageType;


/**
 * 
 */
UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_MagicProjectile : public AAR_ProjectileBase
{
	GENERATED_BODY()

public:
	
	AAR_MagicProjectile();
	
	virtual void LifeSpanExpired() override;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float ImpulseIntensity = 2e5f;

	/** @deprecated idk, but TomLooman deleted this class member variable in 
	 * UE5.6-CourseProject commit (July 21, 2026 at 10:49 PM) Added HandleKilled & Ragdolling for monsters - 2026.7.22 16:23 */
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<UDamageType> DmgTypeClass;
	
	/* ----- Action System Relative ----- */
	
	// Effect applied to 'HitActor' when 'OnImpact'(Hit/Damaged)
	UPROPERTY(EditDefaultsOnly, Category = "ActionSystemRelative")
	TSubclassOf<UAR_Effect> EffectOnHit;	
	
	
	virtual void OnImpact(UPrimitiveComponent* OtherComp, AActor* OtherActor, const FHitResult& Hit) override;
	
};
