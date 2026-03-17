#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "AR_ProjectileBase.generated.h"

UCLASS(Abstract)
class RE_ACTROGUELIKE_API AAR_ProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AAR_ProjectileBase();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;
    
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* MovementComp;
    
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UParticleSystemComponent* EffectComp;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UParticleSystem* ImpactVFX;

	// 防止重复 Explode
	UPROPERTY(VisibleInstanceOnly, Category = "State")
	bool bExploded = false;

	UFUNCTION()
	void OnProjHit(UPrimitiveComponent* ComponentBeenHit, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);
    
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Explode(const FHitResult& Hit);

	virtual void PostInitializeComponents() override;

public:
	FORCEINLINE UProjectileMovementComponent* GetMovementComp() const { return MovementComp; }
};
