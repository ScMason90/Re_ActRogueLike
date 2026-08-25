// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_HealthPotion.h"

// Necessary compilation header files
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActLogChannels.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



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
	
	// Skip Health Potion pickup if doesn't have 'Attribute.Credit' or already full health 
	if (IsValid(ASComp) && ASComp->GetAttribute(SharedGameplayTags::Attribute_Credit) && !UAR_GameplayStatics::IsFullHealth(ASComp))
	{
		float OwnedCredits = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Credit);
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
			// UI Hint for 'lacking of required credits'...?
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, 
				FString::Printf(TEXT("%s Lack of Credit to pickup"), *AR_LOG_LOC()));
		
			// Pickup failed feedback
			UGameplayStatics::PlaySoundAtLocation(
				this, PickupFailedSound, GetActorLocation(), FRotator::ZeroRotator);
		
		}	
	}
}