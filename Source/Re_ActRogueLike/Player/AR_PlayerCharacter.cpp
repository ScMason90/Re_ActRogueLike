// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"
#include "Re_ActRogueLike/Components/AR_InteractionComponent.h"


// Sets default values
AAR_PlayerCharacter::AAR_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
	
	InteractionComponent = CreateDefaultSubobject<UAR_InteractionComponent>(TEXT("InteractionComp"));
	
	AttributeComponent = CreateDefaultSubobject<UAR_AttributeComponent>(TEXT("AttributeComp"));
}

// Called when the game starts or when spawned
void AAR_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAR_PlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	AttributeComponent->OnHealthChanged.AddDynamic(this, &AAR_PlayerCharacter::OnHealthChanged);
	
	TimeToHitParamName = "TimeToHit";
	MuzzleSocketName = "Muzzle_01";
}

void AAR_PlayerCharacter::PrimaryInteract()
{
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryInteraction();
	}
}

void AAR_PlayerCharacter::FireProj(
		TSubclassOf<AActor> ProjClassToSpawn, TObjectPtr<UAnimMontage> AnimMontageToPlay, float TimeBeforeProj,
		TObjectPtr<UNiagaraSystem> EffectWhenCast,
		/* And there are params passing to internal func-AdjustedProjSpawnTransform */
		FName InSocketName, float LineTraceEndOffset)
{
	PlayAnimMontage(AnimMontageToPlay);
	
	UNiagaraFunctionLibrary::SpawnSystemAttached(SharedCastingEffect, GetMesh(), InSocketName, 
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	UGameplayStatics::PlaySound2D(this, SharedCastingSound);
	
	GetWorldTimerManager().SetTimer(TimerHandle_FireProj, 
		[this, ProjClassToSpawn, InSocketName, LineTraceEndOffset]()
		{
			FTransform SpawnTM = AdjustedProjSpawnTransform(InSocketName, LineTraceEndOffset);
	
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;

			GetWorld()->SpawnActor<AActor>(ProjClassToSpawn, SpawnTM, SpawnParams);
	
			/* GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);*/
		}, TimeBeforeProj, false);
}

void AAR_PlayerCharacter::FireMagicProj()	
{FireProj(MagicProjectileClass, SharedFireMontage, 0.4f, SharedCastingEffect,
	MuzzleSocketName, 10000.f);}
void AAR_PlayerCharacter::FireBlackHole()	
{FireProj(BlackHoleProjectileClass, SharedFireMontage, 0.4f, SharedCastingEffect,
	MuzzleSocketName, 10000.f);}
void AAR_PlayerCharacter::FireTeleportProj()	
{FireProj(TeleportProjectileClass, SharedFireMontage, 0.4f, SharedCastingEffect,
	MuzzleSocketName, 1000.f);}

FTransform AAR_PlayerCharacter::AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset)
{
	FVector HandLocation = GetMesh()->GetSocketLocation(InSocketName),
	TraceStart = CameraComponent->GetComponentLocation(),
	TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * LineTraceEndOffset),
	AimShot = TraceEnd;
	
	FHitResult Hit;
	/* Legacy bug - Wrong behavior when LineTrace 'Hit' any actor derived from class AR_MagicProjectile,
	 * It will cause proj actor spawn into an odd direction.May need fix it with specified Trace Channel
	 * ↑ Nah, after 2h struggling on collision channel/preset/object type.I initially repair it.--26.3.13*/ 
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, AimShot, ECC_GameTraceChannel1))
	{
		AimShot = Hit.ImpactPoint;
	}
	/* TODO: Now there is a problem when spawn a MagicProj in a position which camera was too close to something,
	 * The proj will go to the item that closely block around camera.
	 * May need scope iteration to ignore items nearby. */
	FQuat UnderCrossHairQuat = (AimShot - HandLocation).GetSafeNormal().ToOrientationQuat();
	FTransform SpawnTM;
	SpawnTM.SetLocation(HandLocation);
	SpawnTM.SetRotation(UnderCrossHairQuat);
	
	return SpawnTM;
}

// Called every frame
void AAR_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f, Thickness = 5.0f;
	
	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart -= GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction 
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, 
		FColor::Yellow, false, 0.0f, 0, Thickness);
		
	FVector ControllerRotation_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw Controller 'Rotation' ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerRotation_LineEnd, DrawScale,
		FColor::Green, false, 0.0f, 0, Thickness);
}

// Called to bind functionality to input
void AAR_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::Move);
	EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::Look);
	
	EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &ACharacter::Jump);
	EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	
	// TODO: Considering add self-banned(tag, GAS cooldown etc.) to avoid convulsive shooting
	EnhancedInput->BindAction(IA_FireMagicProj, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::FireMagicProj);
	EnhancedInput->BindAction(IA_FireTeleportProj, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::FireTeleportProj);
	EnhancedInput->BindAction(IA_FireBlackHole, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::FireBlackHole);
	
	EnhancedInput->BindAction(IA_PrimaryInteract, ETriggerEvent::Triggered, this, &AAR_PlayerCharacter::PrimaryInteract);
}

void AAR_PlayerCharacter::Move(const FInputActionValue& InValue)
{
	FVector2D InputValue = InValue.Get<FVector2D>();
	
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0;
	
	// Move Forward/Back
	AddMovementInput(ControlRot.Vector(), InputValue.X);
	
	// Sideways
	FVector RightDirection = ControlRot.RotateVector(FVector::RightVector);
	AddMovementInput(RightDirection, InputValue.Y);
}

void AAR_PlayerCharacter::Look(const FInputActionInstance& InValue)
{
	FVector2D InputValue = InValue.GetValue().Get<FVector2D>();
	
	AddControllerPitchInput(InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void AAR_PlayerCharacter::OnHealthChanged(
	AActor* InstigatorActor, UAR_AttributeComponent* OwningComp, float NewHealth, float Delta)
{
	// Flash when damaged
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
	
	// Death logic (RAW)
	if (NewHealth <= 0.0f)
	{
		// Disable Player Input
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			DisableInput(PC);	
		}
		
		// Disable Movement
		GetMovementComponent()->StopActiveMovement();
		
		// Disable Collision
		GetCapsuleComponent()->SetCollisionProfileName("PawnDead");
		GetMesh()->SetCollisionProfileName("PawnDead");
		
		// Stop AI（if this's AI）
		// if (AAIController* AIC = Cast<AAIController>(GetController()))
		// {
		// 	AIC->StopMovement();
		// 	AIC->UnPossess();
		// }

		// Optional
		
		// Play ragdoll	
		// GetMesh()->SetSimulatePhysics(true);

		// Delayed destruction
		// SetLifeSpan(5.0f);
	}
}