// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameModeBase.h"

#include "Re_ActRogueLike/Player/AR_PlayerController.h"

AAR_GameModeBase::AAR_GameModeBase()
{
	PlayerControllerClass = AAR_PlayerController::StaticClass();
	
}
