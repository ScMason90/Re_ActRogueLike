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
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"

static TAutoConsoleVariable<bool> CVarGodMode(TEXT("game.cheat.god"), false,
                                              TEXT("Enable god mode for inf-health... (false = off, true = on)"), ECVF_Cheat);

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
	
	TimeToHitParamName = "TimeToHit";
	
	FOnAttributeChanged& Event = ActionSystemComponent->GetAttributeListener(SharedGameplayTags::Attribute_Health);
	Event.AddUObject(this, &ThisClass::OnHealthChanged);
}

// Called every frame
void AAR_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// -- Rotation Visualization -- //
	constexpr float DrawScale = 100.0f, Thickness = 5.0f;
	
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
	
	EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, 
		&ThisClass::StartAction, SharedGameplayTags::Action_Sprint.GetTag());
	EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this,
		&ThisClass::StopAction, SharedGameplayTags::Action_Sprint.GetTag());
	
	// TODO: Considering add self-banned(tag, GAS cooldown etc.) to avoid convulsive shooting
	EnhancedInput->BindAction(IA_FireMagicProj, ETriggerEvent::Triggered, this, 
		&ThisClass::StartAction, SharedGameplayTags::Action_FireMagicProj.GetTag());
	EnhancedInput->BindAction(IA_FireTeleportProj, ETriggerEvent::Triggered, this, 
		&ThisClass::StartAction, SharedGameplayTags::Action_FireTeleportProj.GetTag());
	EnhancedInput->BindAction(IA_FireBlackHole, ETriggerEvent::Triggered, this, 
		&ThisClass::StartAction, SharedGameplayTags::Action_FireBlackHole.GetTag());
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
#if !UE_BUILD_SHIPPING
	if (bool bEnableGodMode = CVarGodMode.GetValueOnGameThread())
	{
		SetCanBeDamaged(!bEnableGodMode);
		return 0;
	}
#endif
	float ActualDamage =  Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0.0f)
	{
		ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	}
	
	return ActualDamage;
	
}

void AAR_PlayerCharacter::OnHealthChanged(FGameplayTag AttributeTag, float NewHealth, float OldHealth)
{
	float Delta = OldHealth - NewHealth;
	
	if (!IsPlayerDead)
	{
		// Flash when damaged
		if (Delta < 0.0f)
		{
			GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);
		}
	
		// Death logic (RAW), Currently remained as null?
		if (FMath::IsNearlyZero(NewHealth)/*NewHealth <= 0.0f*/)
		{
			// Mark as Dead
			IsPlayerDead = UAR_GameplayStatics::IsDead(ActionSystemComponent);
		
			USkeletalMeshComponent* MeshComp = GetMesh(); 
			// UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
			
			// Disable Player Input
			if (APlayerController* PC = Cast<APlayerController>(GetController())) DisableInput(PC);
		
			// Disable Movement
			GetMovementComponent()->StopActiveMovement();
		
			// Honestly all post-death appearances depend on your game type/design
			
			// Play Death Anim in Anim Class of PlayerCharacter...
			PlayAnimMontage(DeathMontage);
			
			// Disable Collision...
			// CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
			/*↑ if you turn this on, it will be just work if you horizontally dead, 
			 * and you'll fall of the floor if you vertically dead(jump, moving in the air)
			 * Engine level stopping the game is a better one?
			 */
			
			// Optional
		
			// Play ragdoll
			GetWorldTimerManager().SetTimer(TimerHandle_Ragdoll, 
				[this, MeshComp]()
				{
					if (!IsValid(this) || IsPendingKillPending()) return;
					
					MeshComp->SetAllBodiesSimulatePhysics(true);
					MeshComp->SetCollisionProfileName("Ragdoll");
				}, 
				DeathMontageDuration, false);

			// Delayed destruction
			SetLifeSpan(10.0f);
		}
	}
}

void AAR_PlayerCharacter::StartAction(const FInputActionInstance& Instance, FGameplayTag InActionName)
{
	ActionSystemComponent->StartAction(InActionName);
}

void AAR_PlayerCharacter::StopAction(const FInputActionInstance& Instance, FGameplayTag InActionName)
{
	ActionSystemComponent->StopAction(InActionName);
}

void AAR_PlayerCharacter::HealSelf(float Amount /* = 100.0f */)
{
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, Amount, Base);
}
