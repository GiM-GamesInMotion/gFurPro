// Copyright 2017 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(GFurEditor, All, All)

class FGFurEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};