
#include "FurComponentCustomization.h"

#include "FurSplines.h"
#include "FurComponent.h"

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
#include "ScopedTransaction.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Public/ComponentRecreateRenderStateContext.h"
#include "Runtime/AssetRegistry/Public/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

TSharedRef<IDetailCustomization> FFurComponentCustomization::MakeInstance()
{
	return MakeShareable(new FFurComponentCustomization);
}

void FFurComponentCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.EditCategory("gFur Skeletal Mesh");
	DetailBuilder.EditCategory("gFur Static Mesh");

	auto& FurMeshCategory = DetailBuilder.EditCategory("gFur Guides");

	TSharedRef<IPropertyHandle> FurSplinesProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGFurComponent, FurSplines));
	if (FurSplinesProperty->IsValidHandle())
	{
		DetailBuilder.AddPropertyToCategory(FurSplinesProperty);
		CreateFurSplinesAssetWidget(FurMeshCategory.AddCustomRow(FurSplinesProperty->GetPropertyDisplayName(), false), &DetailBuilder);
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

		UPackage *Package = CreatePackage(nullptr, *PackageName);

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

		FScopedTransaction CombTransaction(LOCTEXT("FurComponent_AssingFurSplines", "Assign Fur Splines"));
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
	auto& RenderData = Mesh->RenderData;
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

#undef LOCTEXT_NAMESPACE
