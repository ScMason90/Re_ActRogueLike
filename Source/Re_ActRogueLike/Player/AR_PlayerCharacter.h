// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AR_InteractionComponent.h"
#include "GameFramework/Character.h"
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
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackMontage;
	
	FTimerHandle TimerHandle_PrimaryAttack;

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
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	
	void MoveRight(float Value);
	
	void PrimaryAttack();
	
	void PrimaryAttack_TimeElapsed();
	
	void PrimaryInteract();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
