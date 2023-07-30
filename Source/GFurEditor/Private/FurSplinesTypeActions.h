// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"

class FFurSplinesTypeActions : public FAssetTypeActions_Base
{
public:
	FFurSplinesTypeActions(EAssetTypeCategories::Type InAssetCategory);

	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override;
	virtual FColor GetTypeColor() const override;

	virtual bool IsImportedAsset() const override
	{
		return true;
	}

	/** Collects the resolved source paths for the imported assets */
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;

	// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
