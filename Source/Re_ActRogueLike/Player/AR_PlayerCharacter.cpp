// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRoguelikeType.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"

TAutoConsoleVariable<float> CVarProjectileAdjustmentDebugDrawing(TEXT("game.projectile.DebugDraw"), 0.0f,
	TEXT("Enable projectile aim adjustment debug rendering. (0 = off, > 0 is duration)"), ECVF_Cheat);

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
	
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>(TEXT("AttributeComp"));
}

// Called when the game starts or when spawned
void AAR_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAR_PlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	ActionSystemComponent->OnHealthChanged.AddDynamic(this, &AAR_PlayerCharacter::OnHealthChanged);
	
	TimeToHitParamName = "TimeToHit";
	MuzzleSocketName = "Muzzle_01";
}

void AAR_PlayerCharacter::FireProj(const FFireProjSpawnSourceConfig Config)
{
	PlayAnimMontage(Config.AnimMontageToPlay);
	
	UNiagaraFunctionLibrary::SpawnSystemAttached(Config.EffectWhenCast, GetMesh(), Config.SocketName, 
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	UGameplayStatics::PlaySound2D(this, SharedCastingSFX);
	
	FTimerHandle TimerHandle_FireProj;
	GetWorldTimerManager().SetTimer(TimerHandle_FireProj, 
		[this, Config]()
		{
			FTransform SpawnTM = AdjustedProjSpawnTransform(Config.SocketName, Config.LineTraceEndOffset);
	
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;

			GetWorld()->SpawnActor<AActor>(Config.ProjClassToSpawn, SpawnTM, SpawnParams);
			
		}, Config.TimeBeforeProj, false);
	
	/* GetWorldTimerManager().ClearTimer(TimerHandle_FireProj);*/
}

void AAR_PlayerCharacter::FireMagicProj()	
{FireProj(FFireProjSpawnSourceConfig(MagicProjectileClass, SharedFireMontage, 0.2f, 
	SharedCastingVFX, MuzzleSocketName, 10000.f));}
void AAR_PlayerCharacter::FireBlackHole()	
{FireProj(FFireProjSpawnSourceConfig(BlackHoleProjectileClass, SharedFireMontage, 0.2f, 
	SharedCastingVFX, MuzzleSocketName, 10000.f));}
void AAR_PlayerCharacter::FireTeleportProj()	
{FireProj(FFireProjSpawnSourceConfig(TeleportProjectileClass, SharedFireMontage, 0.2f, 
	SharedCastingVFX, MuzzleSocketName, 1000.f));}

FTransform AAR_PlayerCharacter::AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset)
{
	FVector HandLocation = GetMesh()->GetSocketLocation(InSocketName),
	TraceStart = CameraComponent->GetComponentLocation(),
	TraceEnd = TraceStart + (CameraComponent->GetForwardVector() * LineTraceEndOffset),
	AimShot = TraceEnd;
	
	FHitResult Hit;
	UWorld* World = GetWorld();
	/* Legacy bug - Wrong behavior when LineTrace 'Hit' any actor derived from class AR_MagicProjectile,
	 * It will cause proj actor spawn into an odd direction.May need fix it with specified Trace Channel
	 * ↑ Nah, after 2h struggling on collision channel/preset/object type.I initially repair it.--26.3.13*/ 
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, TraceStart, AimShot, COLLISION_PROJECTILE))
	{
		AimShot = Hit.ImpactPoint;
	}
	/* TODO: Now there is a problem when spawn a MagicProj in a position which camera was too close to something,
	 * The proj will go to the item that closely block around camera.And it's not a stable trace adjustment when
	 * third person camera is close to player character.*/
	FQuat UnderCrossHairQuat = (AimShot - HandLocation).GetSafeNormal().ToOrientationQuat();
	FTransform SpawnTM;
	SpawnTM.SetLocation(HandLocation);
	SpawnTM.SetRotation(UnderCrossHairQuat);
	
#if !UE_BUILD_SHIPPING
	float DebugDrawDuration = CVarProjectileAdjustmentDebugDrawing.GetValueOnGameThread();
	if (DebugDrawDuration > 0.0f)
	{
		// The hit location or trace end
		DrawDebugBox(World, AimShot, FVector(20.0f), FColor::Green, false, DebugDrawDuration);
		
		// Adjustment line trace
		DrawDebugLine(World, TraceStart, TraceEnd, FColor::Green, false, DebugDrawDuration);
		
		// New projectile path
		DrawDebugLine(World, HandLocation, AimShot, FColor::Yellow, false, DebugDrawDuration);
		
		// The original path of the projectile
		DrawDebugLine(World, HandLocation, HandLocation + (GetControlRotation().Vector() * 5000.0f), 
			FColor::Purple, false, DebugDrawDuration);
	}
#endif
	
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

float AAR_PlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ActionSystemComponent->ApplyHealthChange(DamageCauser,-ActualDamage);
	
	return ActualDamage;
	
}

void AAR_PlayerCharacter::OnHealthChanged(
	AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth, float Delta)
{
	if (!IsPlayerDead)
	{
		// Flash when damaged
		if (Delta < 0.0f)
		{
			GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
		}
	
		// Death logic (RAW), Currently remained as null?
		if (FMath::IsNearlyZero(NewHealth)/*NewHealth <= 0.0f*/)
		{
			// Mark as Dead
			IsPlayerDead = ActionSystemComponent->IsDead();
		
			// Disable Player Input
			// if (APlayerController* PC = Cast<APlayerController>(GetController()))
			// {
			// 	DisableInput(PC);	
			// }
			DisableInput(nullptr);
		
			// Disable Movement
			GetMovementComponent()->StopActiveMovement();
		
			// Play Death Anim in Anim Class of PlayerCharacter...
			PlayAnimMontage(DeathMontage);
		
			// Disable Collision...
		
			// Optional
		
			// Play ragdoll	
			// GetMesh()->SetSimulatePhysics(true);

			// Delayed destruction
			// SetLifeSpan(5.0f);
		}
	}
}