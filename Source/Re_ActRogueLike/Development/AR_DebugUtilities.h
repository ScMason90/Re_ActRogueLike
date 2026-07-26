#pragma once


#include "CoreMinimal.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"


#define AR_CHECK_AND_LOG_RET(Assert, Message, ReturnVal) \
	if(!AR_DebugUtils::CheckAndLogImpl(Assert, TEXT(Message), __FUNCTION__, __LINE__)) return ReturnVal;

#define AR_DEBUG_LOC() AR_DebugUtils::MakeDebugLocation(__FUNCTION__, __LINE__)


namespace AR_DebugUtils
{
	inline FString MakeDebugLocation(const char* FunctionName, int Line)
	{
		return FString::Printf(TEXT("[%s: %d]"), ANSI_TO_TCHAR(FunctionName), Line);	
	}
	
	inline bool CheckAndLogImpl(bool Assert, const FString& Message, const char* FuncName, int Line)
	{
		if (Assert)
		{
			UE_LOG(LogARDebug, Error, TEXT("%s, %s"), *MakeDebugLocation(FuncName, Line), *Message);
			return false;
		}
		return true;
	}
	
}