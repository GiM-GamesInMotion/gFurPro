// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurSplinesTypeActions.h"
#include "GFurEditor.h"
#include "FurSplines.h"

//#define LOCTEXT_NAMESPACE "AssetTypeActions"

FFurSplinesTypeActions::FFurSplinesTypeActions(EAssetTypeCategories::Type InAssetCategory)
	:MyAssetCategory(InAssetCategory)
{

}

FText FFurSplinesTypeActions::GetName() const
{
	return LOCTEXT("FurSplinesName", "Fur Splines Asset");
}

UClass* FFurSplinesTypeActions::GetSupportedClass() const
{
	return UFurSplines::StaticClass();
}

FColor FFurSplinesTypeActions::GetTypeColor() const
{
	return FColorList::BrightGold;
}

void FFurSplinesTypeActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (UObject *obj : TypeAssets)
	{
		UFurSplines *asset = Cast<UFurSplines>(obj);

		if (asset != nullptr)
		{
			const FString &filename = asset->ImportFilename;
			if (!filename.IsEmpty())
			{
				OutSourceFilePaths.Add(filename);
			}
		}
	}
}

uint32 FFurSplinesTypeActions::GetCategories()
{
	return MyAssetCategory;
}

#undef LOCTEXT_NAMESPACE