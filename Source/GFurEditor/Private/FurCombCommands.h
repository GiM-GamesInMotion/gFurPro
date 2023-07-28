// Copyright 2023 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Settings/EditorStyleSettings.h"
#include "EditorStyleSet.h"

/** Base set of fur comb commands */
class GFUREDITOR_API FFurCombCommands : public TCommands<FFurCombCommands>
{

public:
	//FFurCombCommands() : TCommands<FFurCombCommands>("FurComb", NSLOCTEXT("Contexts", "FurComb", "Fur Comb"), NAME_None, FEditorStyle::GetStyleSetName()) {}
	FFurCombCommands() : TCommands<FFurCombCommands>("FurComb", NSLOCTEXT("Contexts", "FurComb", "Fur Comb"), NAME_None, FAppStyle::GetAppStyleSetName()) {}


	/**
	* Initialize commands
	*/
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> IncreaseCombSize;
	TSharedPtr<FUICommandInfo> DecreaseCombSize;

	TArray<TSharedPtr<FUICommandInfo>> Commands;
};
