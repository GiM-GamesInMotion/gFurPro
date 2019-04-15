// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "GFurEditor.h"
#include "FurSplinesTypeActions.h"
#include "FurCombEdMode.h"
#include "FurComponentCustomization.h"

#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

void FGFurEditorModule::StartupModule()
{
	//Custom detail views
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type CreaturePackAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FurSplinesAssetCategory")), LOCTEXT("FurSplinesAssetCategory", "Fur Splines"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFurSplinesTypeActions(CreaturePackAssetCategoryBit)));

	FEditorModeRegistry::Get().RegisterMode<FEdModeFurComb>(
		FEdModeFurComb::EM_FurComb,
		NSLOCTEXT("FurComb_Mode", "FurComb_ModeName", "gFur"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.MeshPaintMode", "LevelEditor.MeshPaintMode.Small"),
		true, 1000);

	/** Register detail/property customization */
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("GFurComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FFurComponentCustomization::MakeInstance));
}

void FGFurEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FEdModeFurComb::EM_FurComb);

	/** De-register detail/property customization */
	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomClassLayout("GFurComponent");
	}
}

IMPLEMENT_MODULE(FGFurEditorModule, GFurEditor)
