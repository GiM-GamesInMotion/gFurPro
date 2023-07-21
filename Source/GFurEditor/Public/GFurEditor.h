// Copyright 2023 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
//#include "UObject/Class.h"

#include "Styling/SlateStyle.h"




DECLARE_LOG_CATEGORY_EXTERN(GFurEditor, All, All)

class FGFurEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FSlateStyleSet> StyleSet;
};