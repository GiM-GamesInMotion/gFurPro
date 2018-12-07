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
#include "FurComb.h"
#include "FurCombSettings.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "CombMode"

void SFurCombModeWidget::Construct(const FArguments& InArgs, FFurComb* InComb)
{
	FurComb = InComb;

	SettingsObjects.Add(FurComb->GetFurCombSettings());
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
			FurComb->SetMode(EFurCombMode::AverageLength);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::AverageLength; })),
			NAME_None, LOCTEXT("Mode.FurComb.AverageLength", "Average Length"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip average length"),
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

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::AddRemove);
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::AddRemove; })),
			NAME_None, LOCTEXT("Mode.FurComb.AddRemove", "AddRemove"), LOCTEXT("Mode.FurComb.Tooltip", "TODO tip AddRemove"),
			ColorPaintIcon, EUserInterfaceActionType::ToggleButton);
	}

	return ModeSwitchButtons.MakeWidget();
}

#undef LOCTEXT_NAMESPACE // "PaintModePainter"
