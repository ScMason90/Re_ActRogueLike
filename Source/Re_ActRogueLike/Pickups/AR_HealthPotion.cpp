// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_HealthPotion.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameEngine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"


// Sets default values
AAR_HealthPotion::AAR_HealthPotion()
{
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	PickupMeshComponent->SetupAttachment(RootComponent);
	// Using 'SphereComp' in parent class for collision query instead of subclass.Disable it.
	PickupMeshComponent->SetCollisionProfileName("NoCollision");
	PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Set collision profile(object type), make this in Project Settings, "Overlap" only to Pawn
	OverlapComponent->SetCollisionProfileName("Pickups");
	OverlapComponent->SetupAttachment(PickupMeshComponent);
}

void AAR_HealthPotion::OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnActorOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	UAR_ActionSystemComponent* ASComp = OtherActor->GetComponentByClass<UAR_ActionSystemComponent>();
	
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn) return;
	AController* Controller = Pawn->GetController();
	if (!Controller) return;
	AAR_PlayerState* PS = Controller->GetPlayerState<AAR_PlayerState>();
	if (!PS) return;
	
	// Skip Health Potion pickup if already full health or lacking of required credits
	if (IsValid(ASComp) && !UAR_GameplayStatics::IsFullHealth(ASComp))
	{	// Should we open to AI Pawn for pickup this?
		if (PS->RemoveCredits(CreditCost))
		{
			// TODO: Considering add heal up material flash VFX...or SFX for both damaged and healed?
			// ASComp->ApplyHealthChange(this, HealingAmount);
			ASComp->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, HealingAmount, Base);
		
			// Play(valid context and location) before destroying actor
			UGameplayStatics::PlaySoundAtLocation(
				this, PickupSound, GetActorLocation(), FRotator::ZeroRotator);
		
			// Remove Actor from world, eventually memory will be freed (garbage collection)
			Destroy();
		} else 
		{
			// UI Hint for 'not enough credit'...?
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, 
				TEXT("AAR_HealthPotion::OnActorOverlapped, Lack of Credit to pickup"));
		
			// Play(valid context and location)
			UGameplayStatics::PlaySoundAtLocation(
				this, PickupFailedSound, GetActorLocation(), FRotator::ZeroRotator);
		
		}	
	}
}