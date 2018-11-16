// Copyright 2019 GiM s.r.o. All Rights Reserved.

#include "SFurCombModeWidget.h"

#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Images/SImage.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PropertyCustomizationHelpers.h"
//#include "PaintModeSettingsCustomization.h"TODO
#include "FurComb.h"
#include "FurCombSettings.h"

#include "Modules/ModuleManager.h"
//#include "PaintModeCommands.h"TODO

#define LOCTEXT_NAMESPACE "PaintModePainter"

void SFurCombModeWidget::Construct(const FArguments& InArgs, FFurComb* InComb)
{
	FurComb = InComb;

//	PaintModeSettings = Cast<UPaintModeSettings>(MeshPainter->GetPainterSettings());TODO
	SettingsObjects.Add(FurComb->GetFurCombSettings());
//	SettingsObjects.Add(PaintModeSettings);
	CreateDetailsView();
	
	FMargin StandardPadding(0.0f, 4.0f, 0.0f, 4.0f);
	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.Padding(0.0f)
		[
			SNew(SVerticalBox)
			/** Toolbar containing buttons to switch between different paint modes */
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.HAlign(HAlign_Center)
				[
					CreateToolBarWidget()->AsShared()
				]
			]
				
			/** (Instance) Vertex paint action buttons widget */
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateCombSettingsWidget()->AsShared()
			]
				
			/** Texture paint action buttons widget */
/*			+ SVerticalBox::Slot()TODO
			.AutoHeight()
			[
				CreateTexturePaintWidget()->AsShared()
			]*/

			/** DetailsView containing brush and paint settings */
			+ SVerticalBox::Slot()
			.AutoHeight()				
			[
				SettingsDetailsView->AsShared()
			]
		]
	];
}

void SFurCombModeWidget::CreateDetailsView()
{
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs(
		/*bUpdateFromSelection=*/ false,
		/*bLockable=*/ false,
		/*bAllowSearch=*/ false,
		FDetailsViewArgs::HideNameArea,
		/*bHideSelectionTip=*/ true,
		/*InNotifyHook=*/ nullptr,
		/*InSearchInitialKeyFocus=*/ false,
		/*InViewIdentifier=*/ NAME_None);
	DetailsViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Automatic;
	DetailsViewArgs.bShowOptions = false;
	DetailsViewArgs.bAllowMultipleTopLevelObjects = true;

	SettingsDetailsView = EditModule.CreateDetailView(DetailsViewArgs);
//	SettingsDetailsView->SetRootObjectCustomizationInstance(MakeShareable(new FPaintModeSettingsRootObjectCustomization));
	SettingsDetailsView->SetObjects(SettingsObjects);
}

TSharedPtr<SWidget> SFurCombModeWidget::CreateCombSettingsWidget()
{
	FMargin StandardPadding(0.0f, 4.0f, 0.0f, 4.0f);

	TSharedPtr<SWidget> VertexColorWidget;
	TSharedPtr<SHorizontalBox> VertexColorActionBox;
	TSharedPtr<SHorizontalBox> InstanceColorActionBox;

	static const FText SkelMeshNotificationText = LOCTEXT("SkelMeshAssetPaintInfo", "Paint is directly propagated to Skeletal Mesh Asset(s)");
	static const FText StaticMeshNotificationText = LOCTEXT("StaticMeshAssetPaintInfo", "Paint is directly applied to all LODs");	
		
	SAssignNew(VertexColorWidget, SVerticalBox)
	.Visibility(this, &SFurCombModeWidget::IsVertexPaintModeVisible)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(StandardPadding)
	.HAlign(HAlign_Center)
	[	
		SAssignNew(VertexColorActionBox, SHorizontalBox)
	]
	
	+SVerticalBox::Slot()
	.AutoHeight()
	.Padding(StandardPadding)	
	.HAlign(HAlign_Center)
	[
		SAssignNew(InstanceColorActionBox, SHorizontalBox)
	];

	FToolBarBuilder ColorToolbarBuilder(FurComb->GetUICommandList(), FMultiBoxCustomization::None);
	ColorToolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);
	//TODO
/*	ColorToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Fill, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Fill"));
	ColorToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Propagate, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Propagate"));
	ColorToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Import, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Import"));
	ColorToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Save, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Save"));
*/
	VertexColorActionBox->AddSlot()
	.FillWidth(1.0f)
	[
		ColorToolbarBuilder.MakeWidget()
	];

	FToolBarBuilder InstanceToolbarBuilder(FurComb->GetUICommandList(), FMultiBoxCustomization::None);
	InstanceToolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);
	//TODO
/*	InstanceToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Copy, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Copy"));
	InstanceToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Paste, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Paste"));
	InstanceToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Remove, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Remove"));
	InstanceToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().Fix, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Fix"));
	InstanceToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().PropagateVertexColorsToLODs, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Propagate"));
*/
	InstanceColorActionBox->AddSlot()
	.FillWidth(1.0f)
	[
		InstanceToolbarBuilder.MakeWidget()
	];

	return VertexColorWidget->AsShared();
}
#if 0
TSharedPtr<SWidget> SFurCombModeWidget::CreateTexturePaintWidget()
{
	FMargin StandardPadding(0.0f, 4.0f, 0.0f, 4.0f);
	TSharedPtr<SWidget> TexturePaintWidget;
	TSharedPtr<SHorizontalBox> ActionBox;

	SAssignNew(TexturePaintWidget, SVerticalBox)
	.Visibility(this, &SFurCombModeWidget::IsTexturePaintModeVisible)
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(StandardPadding)
	.HAlign(HAlign_Center)
	[
		SAssignNew(ActionBox, SHorizontalBox)
	];
	 
	FToolBarBuilder TexturePaintToolbarBuilder(FurComb->GetUICommandList(), FMultiBoxCustomization::None);
	TexturePaintToolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);
	//TODO
//	TexturePaintToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().PropagateTexturePaint, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Propagate"));
//	TexturePaintToolbarBuilder.AddToolBarButton(FPaintModeCommands::Get().SaveTexturePaint, NAME_None, FText::GetEmpty(), TAttribute<FText>(), FSlateIcon(FEditorStyle::GetStyleSetName(), "MeshPaint.Save"));

	ActionBox->AddSlot()
	.FillWidth(1.0f)
	[
		TexturePaintToolbarBuilder.MakeWidget()
	];

	return TexturePaintWidget->AsShared();
}
#endif // 0
TSharedPtr<SWidget> SFurCombModeWidget::CreateToolBarWidget()
{
	FToolBarBuilder ModeSwitchButtons(MakeShareable(new FUICommandList()), FMultiBoxCustomization::None);
	{
		FSlateIcon ColorPaintIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.MeshPaintMode.ColorPaint");

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Length);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Length; })),
			NAME_None, LOCTEXT("Mode.FurComb.Length", "Length"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip length"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Bend);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Bend; })),
			NAME_None, LOCTEXT("Mode.FurComb.Blend", "Bend"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip bend"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Clump);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Clump; })),
			NAME_None, LOCTEXT("Mode.FurComb.Clump", "Clump"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip clump"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Twist);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Twist; })),
			NAME_None, LOCTEXT("Mode.FurComb.Twist", "Twist"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip twist"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Noise);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Noise; })),
			NAME_None, LOCTEXT("Mode.FurComb.Noise", "Noise"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip noise"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Relax);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Relax; })),
			NAME_None, LOCTEXT("Mode.FurComb.Relax", "Relax"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip Relax"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);

/*		FSlateIcon WeightPaintIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.MeshPaintMode.WeightPaint");
		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			PaintModeSettings->PaintMode = EPaintMode::Vertices;
			PaintModeSettings->VertexPaintSettings.MeshPaintMode = EMeshPaintMode::PaintWeights;
			SettingsDetailsView->SetObjects(SettingsObjects, true);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return PaintModeSettings->PaintMode == EPaintMode::Vertices && PaintModeSettings->VertexPaintSettings.MeshPaintMode == EMeshPaintMode::PaintWeights; })), NAME_None, LOCTEXT("Mode.VertexWeightPainting", " Weights"), LOCTEXT("Mode.VertexWeight.Tooltip", "Vertex Weight Painting mode allows painting of Vertex Weights"), WeightPaintIcon, EUserInterfaceActionType::ToggleButton);

		FSlateIcon TexturePaintIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.MeshPaintMode.TexturePaint");
		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			PaintModeSettings->PaintMode = EPaintMode::Textures;
			SettingsDetailsView->SetObjects(SettingsObjects, true);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return PaintModeSettings->PaintMode == EPaintMode::Textures; })), NAME_None, LOCTEXT("Mode.TexturePainting", "Textures"), LOCTEXT("Mode.Texture.Tooltip", "Texture Weight Painting mode allows painting on Textures"), TexturePaintIcon, EUserInterfaceActionType::ToggleButton);
		*/
	}

	return ModeSwitchButtons.MakeWidget();
}

EVisibility SFurCombModeWidget::IsVertexPaintModeVisible() const
{
/*	UPaintModeSettings* MeshPaintSettings = (UPaintModeSettings*)MeshPainter->GetPainterSettings();
	return (MeshPaintSettings->PaintMode == EPaintMode::Vertices) ? EVisibility::Visible : EVisibility::Collapsed;*/
	//TODO
	return EVisibility::Visible;
}

EVisibility SFurCombModeWidget::IsTexturePaintModeVisible() const
{
/*	UPaintModeSettings* MeshPaintSettings = (UPaintModeSettings*)MeshPainter->GetPainterSettings();
	return (MeshPaintSettings->PaintMode == EPaintMode::Textures) ? EVisibility::Visible : EVisibility::Collapsed;*/
	//TODO
	return EVisibility::Collapsed;
}

#undef LOCTEXT_NAMESPACE // "PaintModePainter"
