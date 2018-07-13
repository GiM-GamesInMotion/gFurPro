// Copyright 2017 GiM s.r.o. All Rights Reserved.

#include "GFurEditor.h"
#include "FurSplinesTypeActions.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

IMPLEMENT_GAME_MODULE(FGFurEditorModule, FurEditor);

void FGFurEditorModule::StartupModule()
{
//	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom detail views

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type CreaturePackAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FurSplinesAssetCategory")), LOCTEXT("FurSplinesAssetCategory", "Fur Splines"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFurSplinesTypeActions(CreaturePackAssetCategoryBit)));
}

void FGFurEditorModule::ShutdownModule()
{
}
