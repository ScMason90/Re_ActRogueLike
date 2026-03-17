#pragma once

#include "CoreMinimal.h"
#include "AR_ProjectileBase.h"
#include "AR_TeleportProjectile.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_TeleportProjectile : public AAR_ProjectileBase
{
	GENERATED_BODY()

public:
	AAR_TeleportProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void ExplodeAndTeleport();

	FTimerHandle TimerHandle_Explode;
	FTimerHandle TimerHandle_Teleport;
};
