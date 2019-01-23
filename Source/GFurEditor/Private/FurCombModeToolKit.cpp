// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurCombModeToolKit.h"
#include "FurCombEdMode.h"
#include "FurComb.h"

#define LOCTEXT_NAMESPACE "FurCombToolKit"

FFurCombModeToolKit::FFurCombModeToolKit(class FEdModeFurComb* InOwningMode)
	: FurCombEdMode(InOwningMode)
{
}

void FFurCombModeToolKit::Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost)
{
	FModeToolkit::Init(InitToolkitHost);
}

FName FFurCombModeToolKit::GetToolkitFName() const
{
	return FName("FurCombMode");
}

FText FFurCombModeToolKit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Fur Comb");
}

class FEdMode* FFurCombModeToolKit::GetEditorMode() const
{
	return FurCombEdMode;
}

TSharedPtr<SWidget> FFurCombModeToolKit::GetInlineContent() const
{
	return FurCombEdMode->GetFurComb()->GetWidget();
}

#undef LOCTEXT_NAMESPACE // "FurCombToolKit"