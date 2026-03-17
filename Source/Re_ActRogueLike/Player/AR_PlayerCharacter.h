// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Components/AR_InteractionComponent.h"
#include "GameFramework/Character.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"
#include "AR_PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "FireProjectile")
	TSubclassOf<AActor> MagicProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile")
	TSubclassOf<AActor> BlackHoleProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "FireProjectile")
	TSubclassOf<AActor> TeleportProjectileClass;
	// TODO: May considering using separate anim montage for each proj action
	UPROPERTY(EditAnywhere, Category = "FireProjectile")
	UAnimMontage* SharedProjMontage;
	
	FTimerHandle TimerHandle_FireProj;

public:
	// Sets default values for this character's properties
	AAR_PlayerCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;
	
	UPROPERTY(VisibleAnywhere)
	UAR_InteractionComponent* InteractionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAR_AttributeComponent* AttributeComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void PrimaryInteract();
	
	// TODO: May considering introduce different AnimMontage for each proj attack?
	void FireProj(
		TSubclassOf<AActor> ProjClassToSpawn, UAnimMontage* AnimMontageToPlay, float TimeBeforProj,
		/* And there are params passing to internal func-AdjustedProjSpawnTransform */
		FName InSocketName, float LineTraceEndOffset);
	
	void FireMagicProj();
	void FireBlackHole();
	void FireTeleportProj();
	
	FTransform AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
