// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerCharacter.h"

// Necessary compilation header files
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// @Redundant: Already have 'God' command provided by UE5 - true, DamageSystem on our 'PlayerCharacter' wouldn't apply any damage
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

	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>(TEXT("AttributeComp"));
	ActionSystemComponent->SetDefaultAttributeSet(UAR_PlayerAttributeSet::StaticClass());
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
	
}

// Called when the game starts or when spawned
void AAR_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAR_PlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
	
	UAR_GameplayStatics_BIND_ATTR_MULTICAST(
		this, &AAR_PlayerCharacter::OnHealthChanged, ActionSystemComponent, SharedGameplayTags::Attribute_Health);
	
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
	
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, -ActualDamage, Base);
	
	// Damage to Rage Ratio
	// (We could expose '0.f' as a global/general ratio or as an Attribute that can be improved through gameplay)
	const float RageToAdd = DamageAmount * 0.5f;
	ActionSystemComponent->ApplyAttributeChanged(SharedGameplayTags::Attribute_Rage, RageToAdd, Modifier);
	
	return ActualDamage;
	
}

void AAR_PlayerCharacter::OnHealthChanged(FGameplayTag HealthAttributeTag, float NewHealth, float OldHealth)
{
	if (bPlayerDying) return;
	
	// Only execute death logic/appearance immediately when dead (on performance optimization purpose) 
	if (const bool bIsDying = UAR_GameplayStatics::IsDying(ActionSystemComponent))
	{
		bPlayerDying = bIsDying;	// Mark as Dead
		HandleDeath();
		return;		// Remove this line, and migrate this snippet under 'HandleDamaged()' if you want both.
	}

	// 'HandleDamaged()'?
	if (const float Delta = NewHealth - OldHealth; Delta < 0.0f)
	{
		// HitFlashOverlay - Note: this design was rough...'demo/tutorial' only
		GetMesh()->SetOverlayMaterialMaxDrawDistance(0);
		GetMesh()->SetCustomPrimitiveDataFloat(0, GetWorld()->TimeSeconds);
		
		// HitFlashOverlay Ended 
		GetWorldTimerManager().SetTimer(TimerHandle_Overlay, [this]()
		{
			GetMesh()->SetOverlayMaterialMaxDrawDistance(1);
		}, 1.0f/* Overlay Flash Duration*/, false);
		
	}
}

void AAR_PlayerCharacter::HandleDeath()
{
	// Manually stop any possible processing 'Actions' or 'ActionEffects(Buff/Debuff)'
	for (const FGameplayTag& ActiveActionTag : ActionSystemComponent->GetActiveTags())
	{
		ActionSystemComponent->StopAction(ActiveActionTag);
	}
	
	// Remove multicast attribute delegate.
	UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(ActionSystemComponent, SharedGameplayTags::Attribute_Health, DelHandle_OnHealthChanged);
	
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
	 * and you'll fall off the floor if you vertically dead(jump, moving in the air).
	 * Because default collision profile of 'CapsuleComp' is 'Pawn' different by 'CharacterMesh' for 'MeshComp'  
	 * This happened sometimes, not steady.Engine level stopping the game is a better one? */
			
	// Optional
		
	// Play Ragdoll
	FTimerHandle TimerHandle_Ragdoll;
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
