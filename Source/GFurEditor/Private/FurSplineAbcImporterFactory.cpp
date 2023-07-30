// Copyright 2020 GiM s.r.o. All Rights Reserved.

#include "FurSplineAbcImporterFactory.h"
#include "GFurEditor.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EditorReimportHandler.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Misc/PackageName.h"
#include "Editor.h"
#include "AssetRegistryModule.h"
#include "PackageTools.h"
#include "FurSplines.h"
#include "Widgets/Input/SComboBox.h"
#include "Editor/MainFrame/Public/Interfaces/IMainFrameModule.h"
#include "Widgets/Input/SNumericEntryBox.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

PRAGMA_DEFAULT_VISIBILITY_START
THIRD_PARTY_INCLUDES_START
#include "Materials/MaterialInstance.h"
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/Abc/IArchive.h>
#include <Alembic/Abc/IObject.h>
THIRD_PARTY_INCLUDES_END
PRAGMA_DEFAULT_VISIBILITY_END

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#define LOCTEXT_NAMESPACE "GFurEditor"

static void ParseObject(const Alembic::Abc::IObject& InObject, UFurSplines* Splines, const FMatrix& ParentMatrix, const FMatrix& ConversionMatrix, float Scale, bool bCheckGroomAttributes)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(ParseObject);

	// Get MetaData info from current Alembic Object
	const Alembic::Abc::MetaData ObjectMetaData = InObject.getMetaData();
	const uint32 NumChildren = InObject.getNumChildren();

	FMatrix LocalMatrix = ParentMatrix;

	if (Alembic::AbcGeom::ICurves::matches(ObjectMetaData))
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(ParseICurves);

		Alembic::AbcGeom::ICurves Curves = Alembic::AbcGeom::ICurves(InObject, Alembic::Abc::kWrapExisting);
		Alembic::AbcGeom::ICurves::schema_type::Sample Sample = Curves.getSchema().getValue();

		Alembic::Abc::P3fArraySamplePtr Positions = Sample.getPositions();
		Alembic::Abc::Int32ArraySamplePtr NumVertices = Sample.getCurvesNumVertices();

		uint32 NumPoints = Positions ? Positions->size() : 0;
		uint32 NumCurves = NumVertices ? NumVertices->size() : 0; // equivalent to Sample.getNumCurves()

		// Get the starting strand and vertex IDs for this group of ICurves

		FMatrix ConvertedMatrix = ParentMatrix * ConversionMatrix;
		uint32 GlobalIndex = 0;
		uint32 TotalVertices = 0;
		for (uint32 CurveIndex = 0; CurveIndex < NumCurves; ++CurveIndex)
		{
			const uint32 CurveNumVertices = (*NumVertices)[CurveIndex];

			// Check the running total number of vertices and skip processing the rest of the node if there is mismatch with the number of points
			TotalVertices += CurveNumVertices;
			if (TotalVertices > NumPoints)
			{
				// TODO
//				UE_LOG(LogAlembicHairImporter, Warning, TEXT("Curve %u of %u has %u vertices which causes total vertices (%u) to exceed the expected vertices (%u) in ICurves node. This curve and the remaining ones in the node will be skipped."),
//					CurveIndex + 1, NumCurves, CurveNumVertices, TotalVertices, NumPoints);
				break;
			}

			Splines->Index.Add(Splines->Vertices.Num());
			Splines->Count.Add(CurveNumVertices);
			for (uint32 PointIndex = 0; PointIndex < CurveNumVertices; ++PointIndex, ++GlobalIndex)
			{
				Alembic::Abc::P3fArraySample::value_type Position = (*Positions)[GlobalIndex];

				FVector ConvertedPosition = ConvertedMatrix.TransformPosition(FVector(Position.x, Position.y, Position.z));
				Splines->Vertices.Add(ConvertedPosition);
			}
		}

/*		if (bCheckGroomAttributes) TODO
		{
			// Groom attributes as UserProperties on ICurves, 1.3+
			Alembic::AbcGeom::ICompoundProperty Properties = Curves.getSchema().getUserProperties();
			if (Properties)
			{
				if (Properties.getNumProperties() > 0)
				{
					AlembicHairTranslatorUtils::SetGroomAttributes(HairDescription, Properties);
					bCheckGroomAttributes = false;
				}
			}
		}*/
	}
/*	else if (Alembic::AbcGeom::IXform::matches(ObjectMetaData))TODO
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(ParseIXform);

		Alembic::AbcGeom::IXform Xform = Alembic::AbcGeom::IXform(InObject, Alembic::Abc::kWrapExisting);
		Alembic::AbcGeom::XformSample MatrixSample;
		Xform.getSchema().get(MatrixSample);

		// The groom attributes should only be on the first IXform under the top node, no need to check for them once they are found
		if (bCheckGroomAttributes)
		{
			// Groom attributes as UserProperties, 1.1+
			Alembic::AbcGeom::ICompoundProperty Properties = Xform.getSchema().getUserProperties();
			if (Properties)
			{
				if (Properties.getNumProperties() > 0)
				{
					AlembicHairTranslatorUtils::SetGroomAttributes(HairDescription, Properties);
					bCheckGroomAttributes = false;
				}
			}

			// Groom attributes as GeomParams, as fallback to 1.0
			Alembic::AbcGeom::ICompoundProperty ArbParams = Xform.getSchema().getArbGeomParams();
			if (bCheckGroomAttributes && ArbParams)
			{
				if (ArbParams.getNumProperties() > 0)
				{
					AlembicHairTranslatorUtils::SetGroomAttributes(HairDescription, ArbParams);
					bCheckGroomAttributes = false;
				}
			}
		}

		LocalMatrix = ParentMatrix * ConvertAlembicMatrix(MatrixSample.getMatrix());
	}*/

	if (NumChildren > 0)
	{
		for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
		{
			ParseObject(InObject.getChild(ChildIndex), Splines, LocalMatrix, ConversionMatrix, Scale, bCheckGroomAttributes);
		}
	}
}

static bool HasObjectCurves(const Alembic::Abc::IObject& InObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(CheckObject);

	// Get MetaData info from current Alembic Object
	const Alembic::Abc::MetaData ObjectMetaData = InObject.getMetaData();
	const uint32 NumChildren = InObject.getNumChildren();

	if (Alembic::AbcGeom::ICurves::matches(ObjectMetaData))
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(ParseICurves);

		Alembic::AbcGeom::ICurves Curves = Alembic::AbcGeom::ICurves(InObject, Alembic::Abc::kWrapExisting);
		Alembic::AbcGeom::ICurves::schema_type::Sample Sample = Curves.getSchema().getValue();

		Alembic::Abc::P3fArraySamplePtr Positions = Sample.getPositions();
		Alembic::Abc::Int32ArraySamplePtr NumVertices = Sample.getCurvesNumVertices();

		uint32 NumPoints = Positions ? Positions->size() : 0;
		uint32 NumCurves = NumVertices ? NumVertices->size() : 0; // equivalent to Sample.getNumCurves()

		if (NumPoints > 0 && NumCurves > 0)
			return true;
	}

	if (NumChildren > 0)
	{
		for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
		{
			if (HasObjectCurves(InObject.getChild(ChildIndex)))
				return true;
		}
	}
	return false;
}

static bool ImportFurSplinesFromAlembic(const FString& Filename, UFurSplines* FurSplines, int InConversion)
{
	/** Factory used to generate objects*/
	Alembic::AbcCoreFactory::IFactory Factory;
	Alembic::AbcCoreFactory::IFactory::CoreType CompressionType = Alembic::AbcCoreFactory::IFactory::kUnknown;
	/** Archive-typed ABC file */
	Alembic::Abc::IArchive Archive;
	/** Alembic typed root (top) object*/
	Alembic::Abc::IObject TopObject;

	Factory.setPolicy(Alembic::Abc::ErrorHandler::kThrowPolicy);
	Factory.setOgawaNumStreams(12);

	// Extract Archive and compression type from file
	Archive = Factory.getArchive(TCHAR_TO_UTF8(*Filename), CompressionType);
	if (!Archive.valid())
		return NULL;

	// Get Top/root object
	TopObject = Alembic::Abc::IObject(Archive, Alembic::Abc::kTop);
	if (!TopObject.valid())
		return NULL;

//	FMatrix ConversionMatrix = FScaleMatrix::Make(ConversionSettings.Scale) * FRotationMatrix::Make(FQuat::MakeFromEuler(ConversionSettings.Rotation));
	FMatrix ConversionMatrix;
	switch (InConversion)
	{
	case 0:
	default:
		ConversionMatrix = FMatrix::Identity;
		break;
	case 1:
		ConversionMatrix = FMatrix(FPlane(1, 0, 0, 0), FPlane(0, 0, 1, 0), FPlane(0, 1, 0, 0), FPlane(0, 0, 0, 1));
		break;
	}
	FMatrix ParentMatrix = FMatrix::Identity;
	ParseObject(TopObject, FurSplines, ParentMatrix, ConversionMatrix, 1.0f, true);
	FurSplines->ImportFilename = Filename;
	FurSplines->ImportTransformation = InConversion;
	FurSplines->Version = 1;
	FurSplines->UpdateSplines();

	return FurSplines->Vertices.Num() > 0;
}

void SGFurImportOptions::Construct(const FArguments& InArgs)
{
	WidgetWindow = InArgs._WidgetWindow;
	PresetOptions.Reset();

	PresetOptions.Add(MakeShared<int>(0));
	PresetOptions.Add(MakeShared<int>(1));

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
					.Text(LOCTEXT("Import_CurrentFileTitle", "Current File: "))
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
			SNew(SComboBox<TSharedPtr<int>>)
			.OptionsSource(&PresetOptions)
			.OnSelectionChanged_Lambda([this](const TSharedPtr<int>& InOption, ESelectInfo::Type type) {
				if (type != ESelectInfo::Direct)
					ConversionType = *InOption;
			})
			.OnGenerateWidget_Lambda([this](TSharedPtr<int> Item) -> TSharedRef<SWidget>
			{
				TSharedPtr<STextBlock> NewItem = SNew(STextBlock).Text(TextOption(*Item));
				NewItem->SetMargin(FMargin(2));
				return NewItem.ToSharedRef();
			})
			.Content()
			[
				SNew(STextBlock)
				.Text_Lambda([this]() { return TextOption(ConversionType); })
			]
		]

		+ SVerticalBox::Slot()
		.Padding(2)
		.MaxHeight(20.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Threshold", "Threshold"))
				.ToolTipText(LOCTEXT("ThresholdToolTip", "Warning, too high value will assign splines to wrong vertices."))
			]
			+ SHorizontalBox::Slot()
			.Padding(2)
			[
				SNew(SNumericEntryBox<float>)
				.MinValue(0.0f)
				.MaxValue(100.0f)
				.MinSliderValue(0.0f)
				.MaxSliderValue(10.0f)
				.Delta(0.1f)
				.AllowSpin(true)
				.ToolTipText(LOCTEXT("ThresholdToolTip", "Warning, too high value will assign splines to wrong vertices."))
				.Value_Lambda([this]() { return Threshold; })
				.OnValueChanged_Lambda([this](float InValue) {
					Threshold = InValue;
				})
			]
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
				.Text(LOCTEXT("GFurImportOptionWindow_Import", "Import"))
				.IsEnabled(this, &SGFurImportOptions::CanImport)
				.OnClicked(this, &SGFurImportOptions::OnImport)
			]
			+ SUniformGridPanel::Slot(1, 0)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
				.Text(LOCTEXT("SGFurImportOptions_Cancel", "Cancel"))
				.ToolTipText(LOCTEXT("SGFurImportOptions_Cancel_ToolTip", "Cancels importing gFur Splines"))
				.OnClicked(this, &SGFurImportOptions::OnCancel)
			]
		]
	];
}

FText SGFurImportOptions::TextOption(int InOption)
{
	FText text;
	switch (InOption)
	{
	case 0:
	default:
		text = FText::FromString("3ds Max / Blender");
		break;
	case 1:
		text = FText::FromString("Maya");
		break;
	};
	return text;
}

UFurSplineAbcImporterFactory::UFurSplineAbcImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	//bEditAfterNew = true;
	SupportedClass = UFurSplines::StaticClass();

	bEditorImport = true;
	bText = false;
	//	AutoPriority = 0x7fffffff;
	//	ImportPriority = 0x7fffffff;

	Formats.Add(TEXT("abc;Alembic gFur splines"));
}

FText UFurSplineAbcImporterFactory::GetToolTip() const
{
	return FText::FromString("Splines for gFur");
}

bool UFurSplineAbcImporterFactory::FactoryCanImport(const FString& Filename)
{
	/** Factory used to generate objects*/
	Alembic::AbcCoreFactory::IFactory Factory;
	Alembic::AbcCoreFactory::IFactory::CoreType CompressionType = Alembic::AbcCoreFactory::IFactory::kUnknown;
	/** Archive-typed ABC file */
	Alembic::Abc::IArchive Archive;
	/** Alembic typed root (top) object*/
	Alembic::Abc::IObject TopObject;

	Factory.setPolicy(Alembic::Abc::ErrorHandler::kThrowPolicy);
	Factory.setOgawaNumStreams(12);

	// Extract Archive and compression type from file
	Archive = Factory.getArchive(TCHAR_TO_UTF8(*Filename), CompressionType);
	if (!Archive.valid())
		return false;

	// Get Top/root object
	TopObject = Alembic::Abc::IObject(Archive, Alembic::Abc::kTop);
	if (!TopObject.valid())
		return false;

	//	FMatrix ConversionMatrix = FScaleMatrix::Make(ConversionSettings.Scale) * FRotationMatrix::Make(FQuat::MakeFromEuler(ConversionSettings.Rotation));
	return HasObjectCurves(TopObject);
}

UObject* UFurSplineAbcImporterFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	TSharedPtr<SGFurImportOptions> Options;
	ShowImportOptionsWindow(Options, Filename);

	if (!Options->ShouldImport())
	{
		return NULL;
	}

	int ConversionType = Options->GetConversionType().GetValue();
	float Threshold = Options->GetThreshold().GetValue();

	UFurSplines* Result = NewObject<UFurSplines>(InParent, InName, Flags);
	if (ImportFurSplinesFromAlembic(Filename, Result, ConversionType))
	{
		Result->Threshold = Threshold;
		return Result;
	}
	return NULL;
}

bool UFurSplineAbcImporterFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (UFurSplines* Splines = Cast<UFurSplines>(Obj))
	{
		if (!Splines->ImportFilename.IsEmpty())
		{
			OutFilenames.Add(Splines->ImportFilename);
			return true;
		}
	}
	return false;
}

void UFurSplineAbcImporterFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	if (UFurSplines* Splines = Cast<UFurSplines>(Obj))
	{
		if (ensure(NewReimportPaths.Num() == 1))
		{
			Splines->ImportFilename = NewReimportPaths[0];
		}
	}
}

EReimportResult::Type UFurSplineAbcImporterFactory::Reimport(UObject* Obj)
{
	if (UFurSplines* Splines = Cast<UFurSplines>(Obj))
	{
		Splines->Modify();
		Splines->Vertices.Reset();
		Splines->Index.Reset();
		Splines->Count.Reset();
		Splines->Version = 1;
		EReimportResult::Type r = ImportFurSplinesFromAlembic(Splines->ImportFilename, Splines, Splines->ImportTransformation) ? EReimportResult::Succeeded : EReimportResult::Failed;
		Splines->OnSplinesChanged.Broadcast();
		return r;

	}
	return EReimportResult::Failed;
}

int32 UFurSplineAbcImporterFactory::GetPriority() const
{
	return ImportPriority;
}

void UFurSplineAbcImporterFactory::ShowImportOptionsWindow(TSharedPtr<SGFurImportOptions>& Options, FString FilePath) const
{
	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("gFurImportWindowTitle", "gFur Spline Import Options"))
		.SizingRule(ESizingRule::Autosized);

	Window->SetContent
	(
		SAssignNew(Options, SGFurImportOptions).WidgetWindow(Window)
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
