#pragma once


#include "NativeGameplayTags.h"


namespace SharedGameplayTags
{
	// Attributes
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Health);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_HealthMax);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Attribute_Rage);
	
	// Actions
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_FireMagicProj);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_FireTeleportProj);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_FireBlackHole);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Action_Sprint);
	
	// StatusEffects
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatusEffect_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatusEffect_Stunned);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StatusEffect_Burning);


	inline FGameplayTag GlobalAttributeTag = FGameplayTag::RequestGameplayTag("Attribute");

	
	static bool IsAttributeTag(const FGameplayTag& TestTag)
	{
		// String prefix check "TestTag.ToString().StartsWith(TEXT("Attributes."))" This is not recommend as MatchesTag is safer. 
		return TestTag.MatchesTag(GlobalAttributeTag) /*&& !TestTag.MatchesTagExact(GlobalAttributeTag)*/;
	}
}
