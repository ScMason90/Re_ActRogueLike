// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Logging/LogMacros.h"

// Re_Act Log Channels
// -------------------

#define AR_LOG_LOC() FString::Printf(TEXT("[%s %d]"), __FUNCTIONW__, __LINE__)

DECLARE_LOG_CATEGORY_EXTERN(LogGame, Log, All);


// Other...
// --------

#define ONSCREENDEBUGKEY_SPAWNDIRECTOR 100 // - 199
