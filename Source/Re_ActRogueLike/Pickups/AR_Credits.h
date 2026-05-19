// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_PickupActorBase.h"
#include "AR_Credits.generated.h"

UCLASS()
class RE_ACTROGUELIKE_API AAR_Credits : public AAR_PickupActorBase
{
	GENERATED_BODY()

public:
	
	// Sets default values for this actor's properties
	AAR_Credits();

protected:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFXs&Audios")
	TObjectPtr<USoundBase> PickupSound;
	
	UPROPERTY(EditAnywhere, Category = "Credits")
	int32 CreditsAmount;
	
	virtual void OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
