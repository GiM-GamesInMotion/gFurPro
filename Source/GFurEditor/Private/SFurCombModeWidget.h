// Copyright 2019 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

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

protected:
	/** Creates and sets up details view */
	void CreateDetailsView();
	
	/** Returns a widget comprising special UI elements for combing parameters*/
	TSharedPtr<SWidget> CreateCombSettingsWidget();

	/** Returns a widget comprising UI elements for texture painting */
//	TSharedPtr<SWidget> CreateTexturePaintWidget();	

	/** Returns the toolbar widget instance */
	TSharedPtr<SWidget> CreateToolBarWidget();

	/** Getters for whether or not a specific mode is visible */
	EVisibility IsVertexPaintModeVisible() const;
	EVisibility IsTexturePaintModeVisible() const;
protected:	
	/** Objects displayed in the details view */
	TArray<UObject*> SettingsObjects;
	/** Details view for comb settings */
	TSharedPtr<IDetailsView> SettingsDetailsView;
	/** Ptr to comb for which this widget is the ui representation */
	FFurComb* FurComb;
	/** Paint settings instance */
//	UPaintModeSettings* PaintModeSettings;TODO
};
