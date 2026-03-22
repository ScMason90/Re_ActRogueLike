// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerCharacter.h"

#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayInterface.h"


// Sets default values
AAR_PlayerCharacter::AAR_PlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationYaw = false;
	
	InteractionComp = CreateDefaultSubobject<UAR_InteractionComponent>(TEXT("InteractionComp"));
	
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
}

void AAR_PlayerCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	AddMovementInput(ControlRot.Vector(), Value);
}

void AAR_PlayerCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	
	// X - Forward (Red)
	// Y - Right (Green)
	// Z - Up (Blue)
	
	AddMovementInput(RightVector, Value);
}

void AAR_PlayerCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteraction();
	}
}

void AAR_PlayerCharacter::FireProj(
	TSubclassOf<AActor> ProjClassToSpawn, UAnimMontage* AnimMontageToPlay, float TimeBeforeProj,
	/* And there are params passing to internal func-AdjustedProjSpawnTransform */
	FName InSocketName, float LineTraceEndOffset)
{
	PlayAnimMontage(AnimMontageToPlay);
	
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
{FireProj(MagicProjectileClass, SharedProjMontage, 0.4f, 
	"ik_hand_l", 10000.f);}
void AAR_PlayerCharacter::FireBlackHole()	
{FireProj(BlackHoleProjectileClass, SharedProjMontage, 0.4f, 
	"ik_hand_l", 10000.f);}
void AAR_PlayerCharacter::FireTeleportProj()	
{FireProj(TeleportProjectileClass, SharedProjMontage, 0.4f, 
	"ik_hand_l", 1000.f);}

FTransform AAR_PlayerCharacter::AdjustedProjSpawnTransform(FName InSocketName, float LineTraceEndOffset)
{
	FVector HandLocation = GetMesh()->GetSocketLocation(InSocketName),
	TraceStart = CameraComp->GetComponentLocation(),
	TraceEnd = TraceStart + (CameraComp->GetForwardVector() * LineTraceEndOffset),
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
	
	/*  TODO : 请找时间检查 Tom Looman 在哪一版本的github源码仓库中哪一commit里实现了UE5.0以上的EnhancedInput
	 *  输入系统的引入替代了旧系统 (GitHub网站查看commit点击browse repository at this point)*/
	PlayerInputComponent->BindAxis("MoveForward", this, &AAR_PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAR_PlayerCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, 
		this, &AAR_PlayerCharacter::FireMagicProj);
	PlayerInputComponent->BindAction("UltimateAttack", IE_Pressed,
		this, &AAR_PlayerCharacter::FireBlackHole);
	PlayerInputComponent->BindAction("TeleportAttack", IE_Pressed,
		this, &AAR_PlayerCharacter::FireTeleportProj);
	
	
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, 
		this, &AAR_PlayerCharacter::PrimaryInteract);
}

void AAR_PlayerCharacter::OnHealthChanged(
	AActor* InstigatorActor, UAR_AttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
		
		if (NewHealth < 0.0f)
		{
			APlayerController* PC = Cast<APlayerController>(GetController());
			DisableInput(PC);
		}
	}
}