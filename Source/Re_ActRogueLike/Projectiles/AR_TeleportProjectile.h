#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_TeleportProjectile.generated.h"

UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_TeleportProjectile : public AAR_ProjectileBase
{
	GENERATED_BODY()

public:
	
	AAR_TeleportProjectile();

protected:
	
	/* re-usable handle for the initial explosion delay and the teleportation (only 1 timer must be active) */
	FTimerHandle TimerHandle_TeleportProj;
	
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float DetonateDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float TeleportDelay = 0.2f;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ExplodeAndTeleport();
	
	virtual void OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	virtual void OnProjBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
