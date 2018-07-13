// Copyright 2017 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "ModuleManager.h"

class FGFurModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};