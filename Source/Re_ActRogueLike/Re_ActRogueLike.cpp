// Copyright Epic Games, Inc. All Rights Reserved.

#include "Modules/ModuleManager.h"

/**
 * FRe_ActGameModule
 */
class FRe_ActGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}
	
	virtual void ShutdownModule() override
	{
	}
	
};

IMPLEMENT_PRIMARY_GAME_MODULE( FRe_ActGameModule, Re_ActRogueLike, "Re_ActRogueLike" );
