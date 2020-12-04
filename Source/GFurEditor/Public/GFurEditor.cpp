// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "GFurEditor.h"
#include "FurSplinesTypeActions.h"
#include "FurCombEdMode.h"
#include "FurComponentCustomization.h"

#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include <Runtime/Projects/Private/PluginManager.h>

#define LOCTEXT_NAMESPACE "GFurEditor"

void FGFurEditorModule::StartupModule()
{
	StyleSet = MakeShareable(new FSlateStyleSet("gFurStyleSet"));

//	StyleSet->SetContentRoot(FPaths::ProjectPluginsDir() / TEXT("Resources/Editor"));
//	StyleSet->SetCoreContentRoot(FPaths::ProjectPluginsDir() / TEXT("Resources/Editor"));

	FString Content = IPluginManager::Get().FindPlugin(TEXT("GFur"))->GetContentDir();

	StyleSet->Set("gFur.Comb", new FSlateImageBrush(Content / "../Resources/Editor/gFur_icon_40.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Comb.Small", new FSlateImageBrush(Content / "../Resources/Editor/gFur_icon_20.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Length", new FSlateImageBrush(Content / "../Resources/Editor/1LENGHT.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Length.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/1LENGHT.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.AverageLength", new FSlateImageBrush(Content / "../Resources/Editor/40/2AVERAGE.LENGHT.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.AverageLength.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/2AVERAGE.LENGHT.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Bend", new FSlateImageBrush(Content / "../Resources/Editor/40/3BEND.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Bend.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/3BEND.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Clump", new FSlateImageBrush(Content / "../Resources/Editor/40/4.CLUMP.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Clump.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/4.CLUMP.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Twist", new FSlateImageBrush(Content / "../Resources/Editor/40/5TWIST.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Twist.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/5TWIST.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Noise", new FSlateImageBrush(Content / "../Resources/Editor/40/6Noise.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Noise.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/6Noise.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Relax", new FSlateImageBrush(Content / "../Resources/Editor/40/7RELAX.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Relax.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/7RELAX.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.Curl", new FSlateImageBrush(Content / "../Resources/Editor/40/9Curl.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.Curl.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/9Curl.png", FVector2D(20.0f, 20.0f)));
	StyleSet->Set("gFur.AddRemove", new FSlateImageBrush(Content / "../Resources/Editor/40/8ADD.REMOVE.png", FVector2D(40.0f, 40.0f)));
	StyleSet->Set("gFur.AddRemove.Small", new FSlateImageBrush(Content / "../Resources/Editor/20/8ADD.REMOVE.png", FVector2D(20.0f, 20.0f)));

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());

	//Custom detail views
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	EAssetTypeCategories::Type CreaturePackAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("FurSplinesAssetCategory")), LOCTEXT("FurSplinesAssetCategory", "Spline Guides"));

	AssetTools.RegisterAssetTypeActions(MakeShareable(new FFurSplinesTypeActions(CreaturePackAssetCategoryBit)));

	FEditorModeRegistry::Get().RegisterMode<FEdModeFurComb>(
		FEdModeFurComb::EM_FurComb,
		NSLOCTEXT("FurComb_Mode", "FurComb_ModeName", "gFur"),
		FSlateIcon("gFurStyleSet", "gFur.Comb", "gFur.Comb.Small"),
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

	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
}

IMPLEMENT_MODULE(FGFurEditorModule, GFurEditor)
