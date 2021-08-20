
#include "FurComponentCustomization.h"

#include "FurSplines.h"
#include "FurComponent.h"
#include "FurSplineExporter.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "IDetailPropertyRow.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "PropertyRestriction.h"
#include "Engine/Texture2D.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "ScopedTransaction.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Public/ComponentRecreateRenderStateContext.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

static const FFurComponentCustomization* CurrentComponent = nullptr;
static UGFurComponent* CurrentFurComponent = nullptr;
static UFurSplines* CurrentFurSplines = nullptr;
static USkeletalMesh* CurrentSkeletalGrowMesh = nullptr;
static UStaticMesh* CurrentStaticGrowMesh = nullptr;


void SGFurExportOptions::Construct(const FArguments& InArgs)
{
	WidgetWindow = InArgs._WidgetWindow;
	Filename = InArgs._FullPath;

	this->ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2)
		[
			SNew(SBorder)
			.Padding(FMargin(3))
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
					.Text(LOCTEXT("Export_CurrentFileTitle", "Current File: "))
				]
				+ SHorizontalBox::Slot()
				.Padding(5, 0, 0, 0)
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
					.Text(InArgs._FullPath)
				]
			]
		]

		+ SVerticalBox::Slot()
		.Padding(2)
		.MaxHeight(20.0f)
		[
			SNew(SNumericEntryBox<float>)
			.MinValue(0.0f)
			.MaxValue(1000.0f)
			.MinSliderValue(0.1f)
			.MaxSliderValue(10.0f)
			.Delta(0.1f)
			.AllowSpin(true)
			.Value(this, &SGFurExportOptions::GetSplineDensity)
				.OnValueChanged_Lambda([this](float InValue) {
				SplineDensity = InValue;
				Update();
			})
		]

		+ SVerticalBox::Slot()
		.Padding(2)
		.MaxHeight(40.0f)
		[
			SAssignNew(InfoText, STextBlock)
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Right)
		.Padding(2)
		[
			SNew(SUniformGridPanel)
			.SlotPadding(2)
			+ SUniformGridPanel::Slot(0, 0)
			[
				SAssignNew(ImportButton, SButton)
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("GFurExportOptionWindow_Export", "Export"))
				.IsEnabled(this, &SGFurExportOptions::CanExport)
				.OnClicked(this, &SGFurExportOptions::OnExport)
			]
			+ SUniformGridPanel::Slot(1, 0)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("SGFurExportOptions_Cancel", "Cancel"))
				.ToolTipText(LOCTEXT("SGFurExportOptions_Cancel_ToolTip", "Cancels exporting gFur Splines"))
				.OnClicked(this, &SGFurExportOptions::OnCancel)
			]
		]
	];
	Update();
}

void SGFurExportOptions::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (DirtyFlag)
	{
		TimeLeft -= InDeltaTime;
		if (TimeLeft <= 0.0f)
		{
			float CountFactor = GetSplineDensity().GetValue();
			ExportInfo Info = { 0, 0 };
			if (CurrentSkeletalGrowMesh)
				Info = CurrentComponent->ExportHairSplines(Filename.ToString(), CurrentFurSplines, CurrentSkeletalGrowMesh, CurrentFurComponent->MinFurLength, CountFactor, false);
			else if (CurrentStaticGrowMesh)
				Info = CurrentComponent->ExportHairSplines(Filename.ToString(), CurrentFurSplines, CurrentStaticGrowMesh, CurrentFurComponent->MinFurLength, CountFactor, false);
			FText Text = FText::FromString(FString::Printf(TEXT("Groom Spline Count: %i\nStrand Spline Count: %i"), Info.GuideCount, Info.TotalCount - Info.GuideCount));
			InfoText->SetText(Text);
			DirtyFlag = false;
		}
	}
}

bool SGFurExportOptions::CanExport()  const
{
	return true;
}

void SGFurExportOptions::Update()
{
	DirtyFlag = true;
	TimeLeft = 0.5f;
}

TSharedRef<IDetailCustomization> FFurComponentCustomization::MakeInstance()
{
	return MakeShareable(new FFurComponentCustomization);
}

void FFurComponentCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	UGFurComponent* FurComponent = nullptr;
	bool skeletalMesh = true;
	bool staticMesh = true;
	if (FindFurComponent(&DetailBuilder, FurComponent) && FurComponent)
	{
		TArray<USceneComponent*> parents;
		FurComponent->GetParentComponents(parents);
		if (parents.Num())
		{
			skeletalMesh = false;
			staticMesh = true;
			for (USceneComponent* Comp : parents)
			{
				if (Comp->IsA(USkeletalMeshComponent::StaticClass()))
				{
					skeletalMesh = true;
					staticMesh = false;
					break;
				}
			}
		}
	}

	DetailBuilder.EditCategory("gFur Skeletal Mesh").SetCategoryVisibility(skeletalMesh);
	DetailBuilder.EditCategory("gFur Static Mesh").SetCategoryVisibility(staticMesh);

	auto& FurMeshCategory = DetailBuilder.EditCategory("gFur Guides");

	TSharedRef<IPropertyHandle> FurSplinesProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGFurComponent, FurSplines));
	if (FurSplinesProperty->IsValidHandle())
	{
		DetailBuilder.AddPropertyToCategory(FurSplinesProperty);
		CreateFurSplinesAssetWidget(FurMeshCategory.AddCustomRow(FurSplinesProperty->GetPropertyDisplayName(), false), &DetailBuilder);
		ExportFurSplinesAssetWidget(FurMeshCategory.AddCustomRow(FurSplinesProperty->GetPropertyDisplayName(), false), &DetailBuilder);
	}

	DetailBuilder.EditCategory("gFur Shell Settings");
	DetailBuilder.EditCategory("gFur Physics");
}

void FFurComponentCustomization::CreateFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder)
{
	OutWidgetRow
	.WholeRowContent()
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Menu.Background")))
		.Content()
		[
			SNew(SBox)
			.WidthOverride(800)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SBox)
					.WidthOverride(800)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.Padding(2)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("Length", "Length"))
						]
						+ SHorizontalBox::Slot()
						.Padding(2)
						[
							SNew(SNumericEntryBox<float>)
							.MinValue(0.01f)
							.MaxValue(1000.0f)
							.MinSliderValue(1.0f)
							.MaxSliderValue(10.0f)
							.Delta(0.1f)
							.AllowSpin(true)
							.Value(this, &FFurComponentCustomization::GetNewLength)
							.OnValueChanged_Lambda([this](float InValue)
							{
								NewLength = InValue;
							})
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(2)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ControlPoints", "Control Points"))
					]
					+ SHorizontalBox::Slot()
					.Padding(2)
					[
						SNew(SNumericEntryBox<int32>)
						.MinValue(4)
						.MaxValue(128)
						.MinSliderValue(5)
						.MaxSliderValue(15)
						.Delta(1)
						.AllowSpin(true)
						.Value(this, &FFurComponentCustomization::GetNewControlPointCount)
						.OnValueChanged_Lambda([this](int32 InValue)
						{
							NewControlPointCount = InValue;
						})
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(2)
				[
					SNew(SButton)
					.OnClicked_Lambda([this, DetailBuilder]()
						{
							GenerateNewFurSplines(DetailBuilder);
							return FReply::Handled();
						})
					.Text(LOCTEXT("NewSplines", "New Splines"))
					.HAlign(HAlign_Center)
				]
			]
		]
	];
}

void FFurComponentCustomization::GenerateNewFurSplines(IDetailLayoutBuilder* DetailBuilder) const
{
	UGFurComponent* FurComponent = nullptr;
	if (!FindFurComponent(DetailBuilder, FurComponent) || !FurComponent)
		return;
	USkeletalMesh* SkeletalGrowMesh = FurComponent->SkeletalGrowMesh;
	UStaticMesh* StaticGrowMesh = FurComponent->StaticGrowMesh;
	if (!SkeletalGrowMesh && !StaticGrowMesh)
		return;

	// Initialize SaveAssetDialog config
	FSaveAssetDialogConfig SaveAssetDialogConfig;
	SaveAssetDialogConfig.DialogTitleOverride = LOCTEXT("CreateNewSplines", "Create New Splines");
//	SaveAssetDialogConfig.DefaultPath = DefaultPath;
	SaveAssetDialogConfig.DefaultAssetName = "NewSplines";
	SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);
	if (!SaveObjectPath.IsEmpty())
	{
		const FString PackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
		const FString ObjectName = FPackageName::ObjectPathToObjectName(SaveObjectPath);

		UPackage *Package = CreatePackage(*PackageName);

		UFurSplines* FurSplines = FindObject<UFurSplines>(Package, *ObjectName, true);
		bool IsNew;
		if (FurSplines == NULL)
		{
			FurSplines = NewObject<UFurSplines>(Package, UFurSplines::StaticClass(), *ObjectName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | RF_Transactional);
			IsNew = true;
		}
		else
		{
			FurSplines->Vertices.Reset();
			IsNew = false;
		}

		if (SkeletalGrowMesh)
		{
			GenerateSplines(FurSplines, SkeletalGrowMesh, NewControlPointCount, NewLength);
		}
		else
		{
			GenerateSplines(FurSplines, StaticGrowMesh, NewControlPointCount, NewLength);
		}

		FAssetRegistryModule::AssetCreated(FurSplines);
		FurSplines->MarkPackageDirty();

		FScopedTransaction CombTransaction(LOCTEXT("FurComponent_AssingFurSplines", "Assign Spline Guides"));
		FurComponent->Modify();
		if (IsNew)
		{
			FurComponent->FurSplines = FurSplines;
			FComponentRecreateRenderStateContext Context(FurComponent);
		}
		else
		{
			FurSplines->OnSplinesChanged.Broadcast();
		}
	}
}

bool FFurComponentCustomization::FindFurComponent(IDetailLayoutBuilder* DetailBuilder, UGFurComponent*& OutFurComponent) const
{
	int32 AssetCount = 0;
	OutFurComponent = nullptr;

	for (const TWeakObjectPtr<UObject>& SelectedObject : DetailBuilder->GetSelectedObjects())
	{
		if (UGFurComponent* FurComp = Cast<UGFurComponent>(SelectedObject))
		{
			if (AssetCount > 0)
			{
				return false;
			}
			++AssetCount;
			OutFurComponent = FurComp;
		}
	}
	return true;
}

void FFurComponentCustomization::GenerateSplines(UFurSplines* FurSplines, USkeletalMesh* Mesh, int32 ControlPointCount, float Length) const
{
	check(ControlPointCount >= 2);
	auto* SkeletalMeshResource = Mesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	const auto& LodModel = SkeletalMeshResource->LODRenderData[0];
	const auto& SourcePositions = LodModel.StaticVertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodModel.StaticVertexBuffers.StaticMeshVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	FurSplines->Version = 2;
	FurSplines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	FurSplines->ControlPointCount = ControlPointCount;
	uint32 Cnt = 0;
	for (uint32 i = 0; i < VertexCount; i++)
	{
		FVector v = SourcePositions.VertexPosition(i);
		bool found = false;
		for (uint32 k = 0; k < Cnt; k++)
		{
			if (v == FurSplines->Vertices[k])
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			int Index = i * ControlPointCount;
			for (int32 j = 0; j < ControlPointCount; j++)
			{
				float t = j / (float)(ControlPointCount - 1);
				FurSplines->Vertices[Cnt++] = v + SourceVertices.VertexTangentZ(i) * t * Length;
			}
		}
	}
	FurSplines->Vertices.SetNum(Cnt);
}

void FFurComponentCustomization::GenerateSplines(UFurSplines* FurSplines, UStaticMesh* Mesh, int32 ControlPointCount, float Length) const
{
	check(ControlPointCount >= 2);
	auto* RenderData = Mesh->GetRenderData();
	check(RenderData);

	const auto& LodModel = RenderData->LODResources[0];
	const auto& SourcePositions = LodModel.VertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodModel.VertexBuffers.StaticMeshVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	FurSplines->Version = 2;
	FurSplines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	FurSplines->ControlPointCount = ControlPointCount;
	uint32 Cnt = 0;
	for (uint32 i = 0; i < VertexCount; i++)
	{
		FVector v = SourcePositions.VertexPosition(i);
		bool found = false;
		for (uint32 k = 0; k < Cnt; k++)
		{
			if (v == FurSplines->Vertices[k])
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			int Index = i * ControlPointCount;
			for (int32 j = 0; j < ControlPointCount; j++)
			{
				float t = j / (float)(ControlPointCount - 1);
				FurSplines->Vertices[Cnt++] = v + SourceVertices.VertexTangentZ(i) * t * Length;
			}
		}
	}
	FurSplines->Vertices.SetNum(Cnt);
}

void FFurComponentCustomization::ExportFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder)
{
	OutWidgetRow
	.WholeRowContent()
	[
		SNew(SButton)
		.OnClicked_Lambda([this, DetailBuilder]()
			{
				ExportFurSplines(DetailBuilder);
				return FReply::Handled();
			})
		.Text(LOCTEXT("ExportSplines", "Export Splines"))
		.HAlign(HAlign_Center)
	];
}

void FFurComponentCustomization::ExportFurSplines(IDetailLayoutBuilder* DetailBuilder) const
{
	UGFurComponent* FurComponent = nullptr;
	if (!FindFurComponent(DetailBuilder, FurComponent) || !FurComponent)
		return;
	UFurSplines* FurSplines = FurComponent->FurSplines;
	if (!FurSplines)
		return;
	USkeletalMesh* SkeletalGrowMesh = FurComponent->SkeletalGrowMesh;
	UStaticMesh* StaticGrowMesh = FurComponent->StaticGrowMesh;
	if (!SkeletalGrowMesh && !StaticGrowMesh)
		return;

	// Initialize SaveAssetDialog config
	FSaveAssetDialogConfig SaveAssetDialogConfig;
	SaveAssetDialogConfig.DialogTitleOverride = LOCTEXT("ExportFurSplines", "Export gFur Splines");
//	SaveAssetDialogConfig.DefaultPath = DefaultPath;
	SaveAssetDialogConfig.DefaultAssetName = "gFurSplines";
	SaveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FString SaveObjectPath = ContentBrowserModule.Get().CreateModalSaveAssetDialog(SaveAssetDialogConfig);
	if (!SaveObjectPath.IsEmpty())
	{
		const FString PackageName = FPackageName::ObjectPathToPackageName(SaveObjectPath);
		FString Filename;
		if (FPackageName::TryConvertLongPackageNameToFilename(PackageName, Filename, ".abc"))
		{
			CurrentComponent = this;
			CurrentFurComponent = FurComponent;
			CurrentFurSplines = FurSplines;
			CurrentSkeletalGrowMesh = SkeletalGrowMesh;
			CurrentStaticGrowMesh = StaticGrowMesh;

			TSharedPtr<SGFurExportOptions> Options;
			ShowExportOptionsWindow(Options, Filename);

			if (!Options->ShouldExport())
			{
				return;
			}
			CurrentComponent = nullptr;

			float CountFactor = Options->GetSplineDensity().GetValue();
			if (SkeletalGrowMesh)
				ExportHairSplines(Filename, FurSplines, SkeletalGrowMesh, FurComponent->MinFurLength, CountFactor, true);
			else if (StaticGrowMesh)
				ExportHairSplines(Filename, FurSplines, StaticGrowMesh, FurComponent->MinFurLength, CountFactor, true);
		}
	}
}

ExportInfo FFurComponentCustomization::ExportHairSplines(const FString& Filename, UFurSplines* FurSplines, USkeletalMesh* Mesh, float MinFurLength, float CountFactor,
	bool Save)
{
	auto* SkeletalMeshResource = Mesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	TArray<FVector> Points;
	Points.Append(FurSplines->Vertices);

	int32 ControlPointCount = FurSplines->ControlPointCount;
	check(ControlPointCount >= 2);

	int32 GroomSplineCount = Points.Num() / ControlPointCount;

	const auto& LodRenderData = SkeletalMeshResource->LODRenderData[0];
	const auto& SourcePositions = LodRenderData.StaticVertexBuffers.PositionVertexBuffer;
	const auto& SourceUVs = LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer;

	TArray<int32> SplineMap;
	GenerateSplineMap(SplineMap, FurSplines, SourcePositions, MinFurLength);

	TArray<FVector2D> UVs;
	UVs.AddUninitialized(FurSplines->SplineCount());
	for (uint32 Index = 0, Count = SourceUVs.GetNumVertices(); Index < Count; Index++)
	{
		int32 SplineIndex = SplineMap[Index];
		if (SplineIndex >= 0)
			UVs[SplineIndex] = SourceUVs.GetVertexUV(Index, 0);
	}

	TArray<uint32> SourceIndices;
	LodRenderData.MultiSizeIndexContainer.GetIndexBuffer(SourceIndices);

	float CountRemainder = 0.0f;
	for (const auto& RenderSection : LodRenderData.RenderSections)
	{
		for (uint32 t = 0; t < RenderSection.NumTriangles; ++t)
		{
			uint32 Idx[3];
			Idx[0] = SourceIndices[RenderSection.BaseIndex + t * 3];
			Idx[1] = SourceIndices[RenderSection.BaseIndex + t * 3 + 1];
			Idx[2] = SourceIndices[RenderSection.BaseIndex + t * 3 + 2];
			int32 SplineIndices[3];
			SplineIndices[0] = SplineMap[Idx[0]];
			SplineIndices[1] = SplineMap[Idx[1]];
			SplineIndices[2] = SplineMap[Idx[2]];

			if (SplineIndices[0] >= 0 && SplineIndices[1] >= 0 && SplineIndices[2] >= 0)
			{
				FVector Vertices[3];
				Vertices[0] = SourcePositions.VertexPosition(Idx[0]);
				Vertices[1] = SourcePositions.VertexPosition(Idx[1]);
				Vertices[2] = SourcePositions.VertexPosition(Idx[2]);
				FVector2D SrcUVs[3];
				SrcUVs[0] = SourceUVs.GetVertexUV(Idx[0], 0);
				SrcUVs[1] = SourceUVs.GetVertexUV(Idx[1], 0);
				SrcUVs[2] = SourceUVs.GetVertexUV(Idx[2], 0);
				GenerateInterpolatedSplines(Points, UVs, Vertices, SrcUVs, SplineIndices, ControlPointCount, CountRemainder, CountFactor);
			}
		}
	}

	if (Save)
		::ExportFurSplines(Filename, Points, UVs, ControlPointCount, GroomSplineCount);

	ExportInfo Info;
	Info.GuideCount = GroomSplineCount;
	Info.TotalCount = Points.Num() / ControlPointCount;
	return Info;
}

ExportInfo FFurComponentCustomization::ExportHairSplines(const FString& Filename, UFurSplines* FurSplines, UStaticMesh* Mesh, float MinFurLength, float CountFactor,
	bool Save)
{
	auto* StaticMeshResource = Mesh->GetRenderData();
	check(StaticMeshResource);
	const FStaticMeshLODResources& LodRenderData = StaticMeshResource->LODResources[0];

	TArray<FVector> Points;
	Points.Append(FurSplines->Vertices);

	int32 ControlPointCount = FurSplines->ControlPointCount;
	check(ControlPointCount >= 2);

	int32 GroomSplineCount = Points.Num() / ControlPointCount;

	const auto& SourcePositions = LodRenderData.VertexBuffers.PositionVertexBuffer;
	const auto& SourceUVs = LodRenderData.VertexBuffers.StaticMeshVertexBuffer;

	TArray<int32> SplineMap;
	GenerateSplineMap(SplineMap, FurSplines, SourcePositions, MinFurLength);

	TArray<FVector2D> UVs;
	UVs.AddUninitialized(FurSplines->Vertices.Num() * ControlPointCount);
	for (uint32 Index = 0, Count = SourceUVs.GetNumVertices(); Index < Count; Index++)
	{
		int32 SplineIndex = SplineMap[Index];
		UVs[SplineIndex] = SourceUVs.GetVertexUV(Index, 0);
	}

	TArray<uint32> SourceIndices;
	LodRenderData.IndexBuffer.GetCopy(SourceIndices);

	float CountRemainder = 0.0f;
	for (const auto& RenderSection : LodRenderData.Sections)
	{
		for (uint32 t = 0; t < RenderSection.NumTriangles; ++t)
		{
			uint32 Idx[3];
			Idx[0] = SourceIndices[RenderSection.FirstIndex + t * 3];
			Idx[1] = SourceIndices[RenderSection.FirstIndex + t * 3 + 1];
			Idx[2] = SourceIndices[RenderSection.FirstIndex + t * 3 + 2];
			int32 SplineIndices[3];
			SplineIndices[0] = SplineMap[Idx[0]];
			SplineIndices[1] = SplineMap[Idx[1]];
			SplineIndices[2] = SplineMap[Idx[2]];

			if (SplineIndices[0] >= 0 && SplineIndices[1] >= 0 && SplineIndices[2] >= 0)
			{
				FVector Vertices[3];
				Vertices[0] = SourcePositions.VertexPosition(Idx[0]);
				Vertices[1] = SourcePositions.VertexPosition(Idx[1]);
				Vertices[2] = SourcePositions.VertexPosition(Idx[2]);
				FVector2D SrcUVs[3];
				SrcUVs[0] = SourceUVs.GetVertexUV(Idx[0], 0);
				SrcUVs[1] = SourceUVs.GetVertexUV(Idx[1], 0);
				SrcUVs[2] = SourceUVs.GetVertexUV(Idx[2], 0);
				GenerateInterpolatedSplines(Points, UVs, Vertices, SrcUVs, SplineIndices, ControlPointCount, CountRemainder, CountFactor);
			}
		}
	}

	if (Save)
		::ExportFurSplines(Filename, Points, UVs, ControlPointCount, GroomSplineCount);

	ExportInfo Info;
	Info.GuideCount = GroomSplineCount;
	Info.TotalCount = Points.Num() / ControlPointCount;
	return Info;
}

void FFurComponentCustomization::GenerateInterpolatedSplines(TArray<FVector>& Points, TArray<FVector2D>& DestUVs, FVector* Vertices, FVector2D* SrcUVs,
	int32* SplineIndices, int32 ControlPointCount, float& CountRemainder, float CountFactor)
{
	FVector u = Vertices[1] - Vertices[0];
	FVector v = Vertices[2] - Vertices[0];
	FVector n = FVector::CrossProduct(u, v);

	float fCount = n.Size() * CountFactor + CountRemainder;
	int32 Count = (int32)fCount;
	CountRemainder = fCount - Count;

	u.Normalize();
	n.Normalize();
	if (!n.IsNormalized())
		return;
	FMatrix matHelper(u, FVector::CrossProduct(u, n), n, Vertices[0]);
	FMatrix mat = matHelper.InverseFast();

	FVector4 p[3];
	p[0] = mat.TransformPosition(Vertices[0]);
	p[1] = mat.TransformPosition(Vertices[1]);
	p[2] = mat.TransformPosition(Vertices[2]);

	FVector2D min, max;
	min.X = FMath::Min3(p[0].X, p[1].X, p[2].X);
	min.Y = FMath::Min3(p[0].Y, p[1].Y, p[2].Y);
	max.X = FMath::Max3(p[0].X, p[1].X, p[2].X);
	max.Y = FMath::Max3(p[0].Y, p[1].Y, p[2].Y);

	while (Count > 0)
	{
		FVector q;
		q.X = FMath::FRandRange(min.X, max.X);
		q.Y = FMath::FRandRange(min.Y, max.Y);
		q.Z = 0;

		FVector b = FMath::GetBaryCentric2D(q, p[0], p[1], p[2]);
		if (b.X >= 0.0f && b.Y >= 0.0f && b.Z >= 0.0f)
		{
			GenerateInterpolatedSpline(Points, b, SplineIndices, ControlPointCount);
			DestUVs.Add(SrcUVs[0] * b.X + SrcUVs[1] * b.Y + SrcUVs[2] * b.Z);
			Count--;
		}
	}
}

void FFurComponentCustomization::GenerateInterpolatedSpline(TArray<FVector>& Points, const FVector& BarycentricCoords, int32* SplineIndices, int32 ControlPointCount)
{
	Points.AddUninitialized(ControlPointCount);
	FVector* p = &Points[Points.Num() - ControlPointCount];

	int32 Indices[3] = { SplineIndices[0] * ControlPointCount, SplineIndices[1] * ControlPointCount, SplineIndices[2] * ControlPointCount };

	FVector PrevPoint;
	{
		FVector v0 = Points[Indices[0]];
		FVector v1 = Points[Indices[1]];
		FVector v2 = Points[Indices[2]];
		p[0] = PrevPoint = v0 * BarycentricCoords.X + v1 * BarycentricCoords.Y + v2 * BarycentricCoords.Z;
	}

	for (int32 ControlPointIndex = 1; ControlPointIndex < ControlPointCount; ControlPointIndex++)
	{
		int32 PrevControlPointIndex = ControlPointIndex - 1;
		FVector u0 = Points[Indices[0] + PrevControlPointIndex];
		FVector u1 = Points[Indices[1] + PrevControlPointIndex];
		FVector u2 = Points[Indices[2] + PrevControlPointIndex];
		FVector v0 = Points[Indices[0] + ControlPointIndex];
		FVector v1 = Points[Indices[1] + ControlPointIndex];
		FVector v2 = Points[Indices[2] + ControlPointIndex];
		float d0 = (v0 - u0).Size();
		float d1 = (v1 - u1).Size();
		float d2 = (v2 - u2).Size();
		float d = d0 * BarycentricCoords.X + d1 * BarycentricCoords.Y + d2 * BarycentricCoords.Z;

		FVector NewPoint = v0 * BarycentricCoords.X + v1 * BarycentricCoords.Y + v2 * BarycentricCoords.Z;
		FVector dir = NewPoint - PrevPoint;
		dir.Normalize();
		p[ControlPointIndex] = p[PrevControlPointIndex] + dir * d;

		PrevPoint = NewPoint;
	}
}

void FFurComponentCustomization::GenerateSplineMap(TArray<int32>& SplineMap, UFurSplines* FurSplines, const FPositionVertexBuffer& InPositions, float MinFurLength)
{
	uint32 SourceVertexCount = InPositions.GetNumVertices();
	int32 SplineCount = FurSplines->SplineCount();

	uint32 ValidVertexCount = 0;
	float MinLenSquared = FLT_MAX;
	float MaxLenSquared = -FLT_MAX;
	SplineMap.AddUninitialized(SourceVertexCount);

	const int32 Size = 64;
	TArray<int32> Cells;
	TArray<int32> NextIndex;
	Cells.AddUninitialized(Size * Size);
	for (int32 i = 0; i < Size * Size; i++)
		Cells[i] = -1;
	NextIndex.AddUninitialized(SplineCount);

	float MinX = FLT_MAX;
	float MinY = FLT_MAX;
	float MaxX = -FLT_MAX;
	float MaxY = -FLT_MAX;
	for (int32 i = 0; i < SplineCount; i++)
	{
		FVector p = FurSplines->GetFirstControlPoint(i);
		if (p.X < MinX)
			MinX = p.X;
		if (p.Y < MinY)
			MinY = p.Y;
		if (p.X > MaxX)
			MaxX = p.X;
		if (p.Y > MaxY)
			MaxY = p.Y;
	}
	MinX -= 1.0f;
	MinY -= 1.0f;
	MaxX += 1.0f;
	MaxY += 1.0f;
	float FactorWidth = Size / (MaxX - MinX);
	float FactorHeight = Size / (MaxY - MinY);

	for (int32 i = 0; i < SplineCount; i++)
	{
		FVector p = FurSplines->GetFirstControlPoint(i);
		uint32 X = FMath::FloorToInt((p.X - MinX) * FactorWidth);
		uint32 Y = FMath::FloorToInt((p.Y - MinY) * FactorHeight);
		check(X < Size && Y < Size);
		int32 Idx = Cells[Y * Size + X];
		if (Idx == -1)
		{
			Cells[Y * Size + X] = i;
			NextIndex[i] = -1;
		}
		else
		{
			while (NextIndex[Idx] != -1)
				Idx = NextIndex[Idx];
			NextIndex[Idx] = i;
			NextIndex[i] = -1;
		}
	}


	for (uint32 i = 0; i < SourceVertexCount; i++)
	{
		const float Epsilon = 0.1f;
		const float EpsilonSquared = Epsilon * Epsilon;
		FVector p = InPositions.VertexPosition(i);
		int32 BeginX = FMath::Max(FMath::FloorToInt((p.X - Epsilon - MinX) * FactorWidth), 0);
		int32 BeginY = FMath::Max(FMath::FloorToInt((p.Y - Epsilon - MinY) * FactorHeight), 0);
		int32 EndX = FMath::Min(FMath::FloorToInt((p.X + Epsilon - MinX) * FactorWidth), Size - 1);
		int32 EndY = FMath::Min(FMath::FloorToInt((p.Y + Epsilon - MinY) * FactorHeight), Size - 1);
		SplineMap[i] = -1;
		float ClosestDistanceSquared = FLT_MAX;
		int32 ClosestIndex = -1;
		for (int32 Y = BeginY; Y <= EndY; Y++)
		{
			for (int32 X = BeginX; X <= EndX; X++)
			{
				if (X < Size && Y < Size)
				{
					int32 Idx = Cells[Y * Size + X];
					while (Idx != -1)
					{
						FVector s = FurSplines->GetFirstControlPoint(Idx);
						float DistanceSquared = FVector::DistSquared(s, p);
						if (DistanceSquared <= EpsilonSquared)
						{
							FVector s2 = FurSplines->GetLastControlPoint(Idx);
							if (DistanceSquared < ClosestDistanceSquared)
							{
								ClosestDistanceSquared = DistanceSquared;
								ClosestIndex = Idx;
							}
						}
						Idx = NextIndex[Idx];
					}
				}
			}
		}
		SplineMap[i] = ClosestIndex;
		if (ClosestIndex != -1)
		{
			FVector s = FurSplines->GetFirstControlPoint(ClosestIndex);
			FVector s2 = FurSplines->GetLastControlPoint(ClosestIndex);
			float SizeSquared = (s2 - s).SizeSquared();
			if (SizeSquared < MinLenSquared)
				MinLenSquared = SizeSquared;
			if (SizeSquared > MaxLenSquared)
				MaxLenSquared = SizeSquared;
			ValidVertexCount++;
		}
	}
}

void FFurComponentCustomization::ShowExportOptionsWindow(TSharedPtr<SGFurExportOptions>& Options, FString FilePath) const
{
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("gFurExportWindowTitle", "gFur Spline Export Options"))
		.SizingRule(ESizingRule::Autosized);

	Window->SetContent
	(
		SAssignNew(Options, SGFurExportOptions).WidgetWindow(Window)
		.WidgetWindow(Window)
		.FullPath(FText::FromString(FilePath))
	);

	TSharedPtr<SWindow> ParentWindow;

	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		ParentWindow = MainFrame.GetParentWindow();
	}

	FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);
}

#undef LOCTEXT_NAMESPACE
