// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/STextComboBox.h"

class FFurComb;
class UFurCombSettings;
class IDetailsView;

/** Widget representing the state / functionality and settings for FurComb*/
class SFurCombModeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFurCombModeWidget) {}
	SLATE_END_ARGS()

	/** Slate widget construction */
	void Construct(const FArguments& InArgs, FFurComb* InComb);

	void UpdateSelectedPresset(UFurCombSettings* InSettings);

protected:
	/** Creates and sets up details view */
	void CreateDetailsView();
	
	/** Returns the toolbar widget instance */
	TSharedPtr<SWidget> CreateToolBarWidget();

protected:	
	/** Objects displayed in the details view */
	TArray<UObject*> SettingsObjects;
	/** Details view for comb settings */
	TSharedPtr<IDetailsView> SettingsDetailsView;
	/** Ptr to comb for which this widget is the ui representation */
	FFurComb* FurComb;

	TArray<UFurCombSettings*> PresetSettings;
	TArray<TSharedPtr<FString>> PresetOptions;
	TSharedPtr<STextComboBox> PresetComboBox;
	TSharedPtr<SEditableTextBox> NewPresetName;

	void LoadPresets();
	void SelectPreset(const TSharedPtr<FString>& InOption);
	FReply DeletePreset();
	FReply SavePreset();

	bool IsPropertyVisible(const FPropertyAndParent& p);
};
