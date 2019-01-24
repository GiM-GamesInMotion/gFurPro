// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Toolkits/IToolkitHost.h"
#include "Toolkits/BaseToolkit.h"

/**
* Mode Toolkit for the Fur Comb Mode
*/
class FFurCombModeToolKit : public FModeToolkit
{
public:
	FFurCombModeToolKit(class FEdModeFurComb* InOwningMode);

	/** Initializes the geometry mode toolkit */
	virtual void Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

private:
	/** Owning editor mode */
	class FEdModeFurComb* FurCombEdMode;
};
