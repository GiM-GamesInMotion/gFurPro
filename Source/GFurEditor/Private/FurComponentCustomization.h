
#pragma once

#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"
#include "IDetailRootObjectCustomization.h"
#include "PropertyRestriction.h"
#include "Widgets/SWidget.h"

class UFurSplines;
class USkeletalMesh;
class UStaticMesh;

class FFurComponentCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	void CreateFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder) const;
	void GenerateNewFurSplines(IDetailLayoutBuilder* DetailBuilder) const;
	bool FindFurComponent(IDetailLayoutBuilder* DetailBuilder, UGFurComponent*& OutFurComponent) const;

	void GenerateSplines(UFurSplines* FurSplines, USkeletalMesh* Mesh, int ControlPointCount) const;
	void GenerateSplines(UFurSplines* FurSplines, UStaticMesh* Mesh, int ControlPointCount) const;
};
