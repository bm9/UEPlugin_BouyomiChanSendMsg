// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "BouyomiChanSendMsgPrivatePCH.h"


//includeはいるのか不明
#include "Misc/AssertionMacros.h" 

//DEFINE_LOG_CATEGORY(LogBouyomiChanBPAPI);

class FBouyomiChanSendMsg : public IBouyomiChanSendMsg
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FBouyomiChanSendMsg, BouyomiChanSendMsg)



void FBouyomiChanSendMsg::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FBouyomiChanSendMsg::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



