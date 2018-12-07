
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
	float NewLength = 1.0f;
	int NewControlPointCount = 7;

	void CreateFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder);
	void GenerateNewFurSplines(IDetailLayoutBuilder* DetailBuilder) const;
	bool FindFurComponent(IDetailLayoutBuilder* DetailBuilder, UGFurComponent*& OutFurComponent) const;

	void GenerateSplines(UFurSplines* FurSplines, USkeletalMesh* Mesh, int32 ControlPointCount, float Length) const;
	void GenerateSplines(UFurSplines* FurSplines, UStaticMesh* Mesh, int32 ControlPointCount, float Length) const;

	TOptional<float> GetNewLength() const { return NewLength; }
	TOptional<int32> GetNewControlPointCount() const { return NewControlPointCount; }
};
