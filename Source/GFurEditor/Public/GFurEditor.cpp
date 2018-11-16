// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "GFurEditor.h"
#include "FurSplinesTypeActions.h"
#include "FurCombEdMode.h"
#include "FurComponentCustomization.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

IMPLEMENT_GAME_MODULE(FGFurEditorModule, FurEditor);

void FGFurEditorModule::StartupModule()
{
//	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	//Custom detail views

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type CreaturePackAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FurSplinesAssetCategory")), LOCTEXT("FurSplinesAssetCategory", "Fur Splines"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFurSplinesTypeActions(CreaturePackAssetCategoryBit)));



	FEditorModeRegistry::Get().RegisterMode<FEdModeFurComb>(
		FEdModeFurComb::EM_FurComb,
		NSLOCTEXT("FurComb_Mode", "FurComb_ModeName", "Comb"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.MeshPaintMode", "LevelEditor.MeshPaintMode.Small"),
		true, 1000);

	/** Register detail/property customization */
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("GFurComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FFurComponentCustomization::MakeInstance));
	//TODO
/*	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("PaintModeSettings", FOnGetDetailCustomizationInstance::CreateStatic(&FPaintModeSettingsCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("VertexPaintSettings", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FVertexPaintSettingsCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("TexturePaintSettings", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTexturePaintSettingsCustomization::MakeInstance));
	PropertyModule.RegisterCustomPropertyTypeLayout("TexturePaintSettings", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FTexturePaintSettingsCustomization::MakeInstance));

	FModuleManager::Get().LoadModule("MeshPaint");*/
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
	//TODO
/*	FPropertyEditorModule* PropertyModule = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
	if (PropertyModule)
	{
		PropertyModule->UnregisterCustomClassLayout("PaintModeSettings");
		PropertyModule->UnregisterCustomPropertyTypeLayout("VertexPaintSettings");
		PropertyModule->UnregisterCustomPropertyTypeLayout("TexturePaintSettings");
		PropertyModule->UnregisterCustomPropertyTypeLayout("TexturePaintSettings");
	}*/
}
