// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameInstance.h"

void UAR_GameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);
	
	// Non-Fatal assert 'ensure'
	ensure(AliveEnemies.Num() == 0);
}
