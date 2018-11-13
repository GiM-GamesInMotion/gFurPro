// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/** Base set of fur comb commands */
class GFUREDITOR_API FFurCombCommands : public TCommands<FFurCombCommands>
{

public:
	FFurCombCommands() : TCommands<FFurCombCommands>("FurComb", NSLOCTEXT("Contexts", "FurComb", "Fur Comb"), NAME_None, FEditorStyle::GetStyleSetName()) {}

	/**
	* Initialize commands
	*/
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> IncreaseCombSize;
	TSharedPtr<FUICommandInfo> DecreaseCombSize;

	/* TODO
	TSharedPtr<FUICommandInfo> NextTexture;
	TSharedPtr<FUICommandInfo> PreviousTexture;
	TSharedPtr<FUICommandInfo> CommitTexturePainting;

	TSharedPtr<FUICommandInfo> Copy;
	TSharedPtr<FUICommandInfo> Paste;
	TSharedPtr<FUICommandInfo> Remove;
	TSharedPtr<FUICommandInfo> Fix;
	TSharedPtr<FUICommandInfo> Fill;
	TSharedPtr<FUICommandInfo> Propagate;
	TSharedPtr<FUICommandInfo> Import;
	TSharedPtr<FUICommandInfo> Save;

	TSharedPtr<FUICommandInfo> SwitchForeAndBackgroundColor;
	TSharedPtr<FUICommandInfo> CycleToNextLOD;
	TSharedPtr<FUICommandInfo> CycleToPreviousLOD;

	TSharedPtr<FUICommandInfo> PropagateTexturePaint;
	TSharedPtr<FUICommandInfo> SaveTexturePaint;

	TSharedPtr<FUICommandInfo> PropagateVertexColorsToLODs;
	*/

	TArray<TSharedPtr<FUICommandInfo>> Commands;
};
