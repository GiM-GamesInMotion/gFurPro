// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "FurCombCommands.h"

#define LOCTEXT_NAMESPACE "LevelFurCombCommands"

void FFurCombCommands::RegisterCommands()
{
	UI_COMMAND(IncreaseCombSize, "IncreaseComb", "Increases comb size", EUserInterfaceActionType::Button, FInputChord(EKeys::RightBracket, EModifierKey::Control));
	Commands.Add(IncreaseCombSize);

	UI_COMMAND(DecreaseCombSize, "DecreaseComb", "Decreases comb size", EUserInterfaceActionType::Button, FInputChord(EKeys::LeftBracket, EModifierKey::Control));
	Commands.Add(DecreaseCombSize);
}

#undef LOCTEXT_NAMESPACE

