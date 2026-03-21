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
	UPROPERTY(VisibleAnywhere, Category = "References")
	AActor* InstigatorPawnActorRef;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	virtual void PostInitializeComponents() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void LifeSpanExpired() override;
	
	virtual void OnProjBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void OnProjHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, FVector NormalisedImpulse, const FHitResult& Hit) override;
};
