// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_Effect.h"
#include "AR_Effect_Overwhelm.generated.h"


/**
 * TODO: Create a new Specified 'StatusEffectManager/Trigger' GameplayTag category? 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_Effect_Overwhelm : public UAR_Effect
{
	GENERATED_BODY()
	
public:
	
	UAR_Effect_Overwhelm();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Overwhelm")
	int32 MaxOverwhelmPoints;
	
	UPROPERTY(Transient)
	int32 CurrentOverwhelmPoints;
	
	// Utility manage 'Stun' through this 'Overwhelm' logic effect class for both Player and Enemy?
	UPROPERTY(EditDefaultsOnly, Category = "Overwhelm")
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
