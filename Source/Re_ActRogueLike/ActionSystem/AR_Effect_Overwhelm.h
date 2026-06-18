// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_Effect.h"
#include "AR_Effect_Overwhelm.generated.h"


/**	Temporarily make this 'EffectTrigger' responsible for both 'Stun' applied to Minion.Only for test
 * TODO: Create a new Specified 'StatusEffectManager/Trigger' GameplayTag category? 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_Effect_Overwhelm : public UAR_Effect
{
	GENERATED_BODY()
	
public:
	
	UAR_Effect_Overwhelm();
	
protected:
	
	/* Temp for now and Test only */
	/* ---------- Stun -----------*/
	
	UPROPERTY(EditDefaultsOnly, Category = "Overwhelm | Stun")
	int32 OnStunThreshold;	// MaxOverwhelmPoints, 'Hit Counter'
	
	UPROPERTY(Transient)
	int32 CurHit;	// CurrentOverwhelmPoints, 'Current Hit Counter'
	
	// Utility class manage 'Stun' through this 'Overwhelm' logic effect class for both Player and Enemy?
	UPROPERTY(EditDefaultsOnly, Category = "Overwhelm | Stun")
	TSubclassOf<UAR_Action> StunnedEffectClass;
	
	/** StunTrigger logic rough only for testing.Integrating with 'DamageSystem' in ue5 since that we implemented it. 
	 * May switch to 'ActionSystem' by subscribing 'FOnAttributeChanged' multicast delegate from OwningASComp.
	 */
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);
	
public:
	
	virtual void StartAction_Implementation() override;
	virtual void StopAction_Implementation() override;
	
};
