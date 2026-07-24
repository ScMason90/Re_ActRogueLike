#include "SharedGameplayTags.h"


namespace SharedGameplayTags
{
	// Attributes
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Health, "Attribute.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_HealthMax, "Attribute.HealthMax");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Rage, "Attribute.Rage");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Credit, "Attribute.Credit");
	
	// Actions
	UE_DEFINE_GAMEPLAY_TAG(Action_FireMagicProj, "Action.FireMagicProj");
	UE_DEFINE_GAMEPLAY_TAG(Action_FireTeleportProj, "Action.FireTeleportProj");
	UE_DEFINE_GAMEPLAY_TAG(Action_FireBlackHole, "Action.FireBlackHole");
	UE_DEFINE_GAMEPLAY_TAG(Action_Sprint, "Action.Sprint");
	
	// StatusEffects
	UE_DEFINE_GAMEPLAY_TAG(StatusEffect_Sprinting, "StatusEffect.Sprinting");
	UE_DEFINE_GAMEPLAY_TAG(StatusEffect_Stunned, "StatusEffect.Stunned");
	UE_DEFINE_GAMEPLAY_TAG(StatusEffect_Burning, "StatusEffect.Burning");
	
}
