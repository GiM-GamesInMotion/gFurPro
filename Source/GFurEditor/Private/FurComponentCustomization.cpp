
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
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"

#define LOCTEXT_NAMESPACE "GFurEditor"

TSharedRef<IDetailCustomization> FFurComponentCustomization::MakeInstance()
{
	return MakeShareable(new FFurComponentCustomization);
}

void FFurComponentCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto& FurMeshCategory = DetailBuilder.EditCategory("gFur Mesh");

	TSharedRef<IPropertyHandle> FurSplinesProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UGFurComponent, FurSplines));
	if (FurSplinesProperty->IsValidHandle())
	{
		DetailBuilder.AddPropertyToCategory(FurSplinesProperty);
		CreateFurSplinesAssetWidget(FurMeshCategory.AddCustomRow(FurSplinesProperty->GetPropertyDisplayName(), false), &DetailBuilder);
	}
}

void FFurComponentCustomization::CreateFurSplinesAssetWidget(FDetailWidgetRow& OutWidgetRow, IDetailLayoutBuilder* DetailBuilder) const
{
	OutWidgetRow
	.ValueContent()
	[
		SNew(SButton)
		.OnClicked_Lambda([this, DetailBuilder]()
			{
				GenerateNewFurSplines(DetailBuilder);
				return FReply::Handled();
			})
		.Text(LOCTEXT("NewSplines", "New Splines"))
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
		FPackageName::ObjectPathToObjectName(SaveObjectPath);

		UPackage *Package = CreatePackage(nullptr, *PackageName);
		UFurSplines* FurSplines = NewObject<UFurSplines>(Package, UFurSplines::StaticClass(), *FString("FurSplines"), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

		int ControlPointCount = 16;
		if (SkeletalGrowMesh)
			GenerateSplines(FurSplines, SkeletalGrowMesh, ControlPointCount);
		else
			GenerateSplines(FurSplines, StaticGrowMesh, ControlPointCount);

		FAssetRegistryModule::AssetCreated(FurSplines);
		FurSplines->MarkPackageDirty();

		FScopedTransaction CombTransaction(LOCTEXT("FurComponent_AssingFurSplines", "Assign Fur Splines"));
		FurComponent->Modify();
		FurComponent->FurSplines = FurSplines;
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

void FFurComponentCustomization::GenerateSplines(UFurSplines* FurSplines, USkeletalMesh* Mesh, int ControlPointCount) const
{
	check(ControlPointCount >= 2);
	auto* SkeletalMeshResource = Mesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	const auto& LodModel = SkeletalMeshResource->LODRenderData[0];
	const auto& SourcePositions = LodModel.StaticVertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodModel.StaticVertexBuffers.StaticMeshVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	FurSplines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	FurSplines->Index.AddUninitialized(VertexCount);
	FurSplines->Count.AddUninitialized(VertexCount);
	for (uint32 i = 0; i < VertexCount; i++)
	{
		int Index = i * ControlPointCount;
		FurSplines->Index[i] = Index;
		FurSplines->Count[i] = ControlPointCount;
		for (int j = 0; j < ControlPointCount; j++)
		{
			float t = j / (float)(ControlPointCount - 1);
			FurSplines->Vertices[Index + j] = SourcePositions.VertexPosition(i) + SourceVertices.VertexTangentZ(i) * t;
		}
	}

	for (int i = 0, c = ControlPointCount * VertexCount; i < c; i++)
		FurSplines->Vertices[i].Y = -FurSplines->Vertices[i].Y;
}

void FFurComponentCustomization::GenerateSplines(UFurSplines* FurSplines, UStaticMesh* Mesh, int ControlPointCount) const
{
	check(ControlPointCount >= 2);
	auto& RenderData = Mesh->RenderData;
	check(RenderData);

	const auto& LodModel = RenderData->LODResources[0];
	const auto& SourcePositions = LodModel.VertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodModel.VertexBuffers.StaticMeshVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	FurSplines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	FurSplines->Index.AddUninitialized(VertexCount);
	FurSplines->Count.AddUninitialized(VertexCount);
	for (uint32 i = 0; i < VertexCount; i++)
	{
		int Index = i * ControlPointCount;
		FurSplines->Index[i] = Index;
		FurSplines->Count[i] = ControlPointCount;
		for (int j = 0; j < ControlPointCount; j++)
		{
			float t = j / (float)(ControlPointCount - 1);
			FurSplines->Vertices[Index + j] = SourcePositions.VertexPosition(i) + SourceVertices.VertexTangentZ(i) * t;
		}
	}

	for (int i = 0, c = ControlPointCount * VertexCount; i < c; i++)
		FurSplines->Vertices[i].Y = -FurSplines->Vertices[i].Y;
}

#undef LOCTEXT_NAMESPACE
