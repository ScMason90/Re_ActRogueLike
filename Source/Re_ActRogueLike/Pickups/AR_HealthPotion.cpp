// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_HealthPotion.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"


// Sets default values
AAR_HealthPotion::AAR_HealthPotion()
{
	// Using 'SphereComp' in parent class for collision query instead of subclass...
	
	// Set collision profile(object type), make this in Project Settings, "Overlap" only to Pawn
	OverlapComponent->SetCollisionProfileName("Pickups");

}

void AAR_HealthPotion::OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnActorOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	UAR_ActionSystemComponent* ASComp = OtherActor->FindComponentByClass<UAR_ActionSystemComponent>();
	
	// Skip Health Potion pickup if already full health or lacking of required credits
	if (IsValid(ASComp) && !UAR_GameplayStatics::IsFullHealth(ASComp))
	{
		float OwnedCredits = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Credit);	// Note that 'MinionRanged' doesn't have Attribute.Credit
		if (OwnedCredits >= CreditCost)	
		{
			// TODO: Considering add heal up material flash VFX...or SFX for both damaged and healed?
			ASComp->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, HealingAmount, Base);
			ASComp->ApplyAttributeChanged(SharedGameplayTags::Attribute_Credit, -CreditCost, Base);
			
			// Pickup success feedback
			UGameplayStatics::PlaySoundAtLocation(
				this, PickupSound, GetActorLocation(), FRotator::ZeroRotator);
			
			Destroy();
		} else 
		{
			// UI Hint for 'not enough credit'...?
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, 
				TEXT("AAR_HealthPotion::OnActorOverlapped, Lack of Credit to pickup"));
		
			// Pickup failed feedback
			UGameplayStatics::PlaySoundAtLocation(
				this, PickupFailedSound, GetActorLocation(), FRotator::ZeroRotator);
		
		}	
	}
}