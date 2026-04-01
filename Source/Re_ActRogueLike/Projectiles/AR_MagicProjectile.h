#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_MagicProjectile.generated.h"

class UDamageType;
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
	TSubclassOf<UDamageType> DmgTypeClass;
	
	virtual void OnImpact(AActor* OtherActor, const FHitResult& Hit) override;
};
