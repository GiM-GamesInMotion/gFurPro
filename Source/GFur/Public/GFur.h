// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FGFurModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};