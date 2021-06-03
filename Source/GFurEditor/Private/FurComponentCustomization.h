
#pragma once

#include "IDetailCustomization.h"
#include "IPropertyTypeCustomization.h"
#include "IDetailRootObjectCustomization.h"
#include "PropertyRestriction.h"
#include "Widgets/SWidget.h"

class UFurSplines;
class USkeletalMesh;
class UStaticMesh;
class UGFurComponent;

struct ExportInfo
{
	int32 GuideCount;
	int32 TotalCount;
};

class SGFurExportOptions : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGFurExportOptions)
		: _WidgetWindow()
		{}

		SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_ARGUMENT(FText, FullPath)				
	SLATE_END_ARGS()

	TOptional<float> GetSplineDensity() const
	{
		return SplineDensity;
	}

public:
	void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnExport()
	{
		bShouldExport = true;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FReply OnCancel()
	{
		bShouldExport = false;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			return OnCancel();
		}

		return FReply::Unhandled();
	}

	bool ShouldExport() const
	{
		return bShouldExport;
	}


	SGFurExportOptions()
	: bShouldExport(false)
	{}

	void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	bool CanExport() const;

private:
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr< SButton > ImportButton;
	bool			bShouldExport;
	TSharedPtr<IDetailsView> DetailsView;
	float SplineDensity = 0.5f;
	TSharedPtr< STextBlock > InfoText;
	FText Filename;

	bool DirtyFlag = true;
	float TimeLeft = 0.0f;

	void Update();
};

class FFurComponentCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	float NewLength = 3.0f;
	int NewControlPointCount = 7;

	void CreateFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder);
	void GenerateNewFurSplines(IDetailLayoutBuilder* DetailBuilder) const;
	bool FindFurComponent(IDetailLayoutBuilder* DetailBuilder, UGFurComponent*& OutFurComponent) const;

	void GenerateSplines(UFurSplines* FurSplines, USkeletalMesh* Mesh, int32 ControlPointCount, float Length) const;
	void GenerateSplines(UFurSplines* FurSplines, UStaticMesh* Mesh, int32 ControlPointCount, float Length) const;

	TOptional<float> GetNewLength() const { return NewLength; }
	TOptional<int32> GetNewControlPointCount() const { return NewControlPointCount; }

	void ExportFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder);
	void ExportFurSplines(IDetailLayoutBuilder* DetailBuilder) const;

	static ExportInfo ExportHairSplines(const FString& filename, UFurSplines* FurSplines, USkeletalMesh* Mesh, float MinFurLength, float CountFactor, bool Save);
	static ExportInfo ExportHairSplines(const FString& filename, UFurSplines* FurSplines, UStaticMesh* Mesh, float MinFurLength, float CountFactor, bool Save);

	static void GenerateInterpolatedSplines(TArray<FVector>& Points, TArray<FVector2D>& DestUVs, FVector* Vertices, FVector2D* SrcUVs,
		int32* SplineIndices, int32 ControlPointCount, float& CountRemainder, float CountFactor);
	static void GenerateInterpolatedSpline(TArray<FVector>& Points, const FVector& BarycentricCoords, int32* SplineIndices, int32 ControlPointCount);
	static void GenerateSplineMap(TArray<int32>& SplineMap, UFurSplines* FurSplines, const class FPositionVertexBuffer& InPositions, float MinFurLength);

	void ShowExportOptionsWindow(TSharedPtr<SGFurExportOptions>& Options, FString FilePath) const;

	friend class SGFurExportOptions;
};
