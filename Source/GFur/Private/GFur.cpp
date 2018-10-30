// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "GFur.h"

#define LOCTEXT_NAMESPACE "FGFurModule"

void FGFurModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGFurModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGFurModule, GFur)