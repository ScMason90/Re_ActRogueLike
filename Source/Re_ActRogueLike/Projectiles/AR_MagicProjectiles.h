#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_MagicProjectiles.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_MagicProjectiles : public AAR_ProjectileBase
{
	GENERATED_BODY()

public:
	AAR_MagicProjectiles();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	virtual void LifeSpanExpired() override;

	UFUNCTION()
	void OnComponentBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
