// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AR_PlayerCharacter.generated.h"

struct FInputActionInstance;
struct FInputActionValue;
class UAR_AttributeComponent;
class UAR_InteractionComponent;
class UAnimMontage;
class UCameraComponent;
class UInputAction;
class UNiagaraSystem;
class UParticleSystem;
class USpringArmComponent;

USTRUCT(BlueprintType)
struct FFireProjSpawnSourceConfig
{
	GENERATED_BODY()
	
public:
	// Default constructor that forces parameter passing.
	FFireProjSpawnSourceConfig(TSubclassOf<AActor> InProjClassToSpawn, 
		TObjectPtr<UAnimMontage> InAnimMontageToPlay,float InTimeBeforeProj,
		TObjectPtr<UNiagaraSystem> InEffectWhenCast, FName InSocketName, float InLineTraceEndOffset)
	:ProjClassToSpawn(InProjClassToSpawn), AnimMontageToPlay(InAnimMontageToPlay), TimeBeforeProj(InTimeBeforeProj),
	EffectWhenCast(InEffectWhenCast), SocketName(InSocketName), LineTraceEndOffset(InLineTraceEndOffset){}

	// The UE reflection system requires a default constructor (which can be empty).
	FFireProjSpawnSourceConfig() = default;
	
	/*------------------struct variables----------------------*/
	TSubclassOf<AActor> ProjClassToSpawn;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> AnimMontageToPlay;
	
	float TimeBeforeProj = 0.0f;
	
	UPROPERTY()
	TObjectPtr<UNiagaraSystem> EffectWhenCast;
	
	/* And there are params passing to internal func-AdjustedProjSpawnTransform */
	FName SocketName;
	float LineTraceEndOffset = 10000.0f;
};

/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API AAR_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	/* -------------- Projectile Sources -------------------
	 * Three projectile class/instance in cpp/bp are all ultimately derived from AAcotr class. 
	 * For keeping relative snippet work smoothly, no need to change the variable type. */ 
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> MagicProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> BlackHoleProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | SpawnSources")
	TSubclassOf<AActor> TeleportProjectileClass;
	
	/* Projectile - Anim&Effect */
	// TODO: May considering using separate 'anim montage & casting effect & spawn socket' for each proj action
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | Anim&Effect")
	TObjectPtr<UAnimMontage> SharedFireMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | Anim&Effect")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | Anim&Effect")// NiagaraSystem played during attack animation
	TObjectPtr<UNiagaraSystem> SharedCastingVFX;
	
	UPROPERTY(VisibleAnywhere, Category = "FireProjectile | Anim&Effect")
	FName MuzzleSocketName;
	
	/* Projectile - Sounds&Audios */
	UPROPERTY(EditDefaultsOnly, Category = "FireProjectile | Sounds&Audios")
	TObjectPtr<USoundBase> SharedCastingSFX;
	
	/* ---------------------- Input Action ----------------------- */
	/* Movements */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;
	
	/* Interaction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_PrimaryInteract;
	
	/* Combat and fight */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireMagicProj;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireTeleportProj;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_FireBlackHole;
	
	/* -------------Material Relative---------------- */
	/* VisibleAnywhere = read-only, still useful to view in-editor and enforce a convention. */
	UPROPERTY(VisibleAnywhere, Category = "Reaction | OnHealthChange")
	FName TimeToHitParamName;

public:
	// Sets default values for this character's properties
	AAR_PlayerCharacter();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostInitializeComponents() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAR_AttributeComponent> AttributeComponent;
	
	void Move(const FInputActionValue& InValue);
	void Look(const FInputActionInstance& InValue);
	
	// TODO: May considering introduce different AnimMontage for each proj attack?
	void FireProj(const FFireProjSpawnSourceConfig Config);
	
	void FireMagicProj();
	void FireBlackHole();
	void FireTeleportProj();
	
	FTransform AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset);
	
	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor, UAR_AttributeComponent* OwningComp, float NewHealth, float Delta);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
