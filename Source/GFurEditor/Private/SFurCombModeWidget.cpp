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
#include "Widgets/Input/STextComboBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "FurComb.h"
#include "FurCombSettings.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "CombMode"

void SFurCombModeWidget::Construct(const FArguments& InArgs, FFurComb* InComb)
{
	FurComb = InComb;

	LoadPresets();

	SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
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
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 4.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				.Padding(2.0f, 0.0f)
				[
					SAssignNew(PresetComboBox, STextComboBox)
					.OptionsSource(&PresetOptions)
					.OnSelectionChanged_Lambda([this](const TSharedPtr<FString>& InOption, ESelectInfo::Type type) { if (type != ESelectInfo::Direct) SelectPreset(InOption); })
				]
				+ SHorizontalBox::Slot()
				.Padding(2.0f, 0.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("GFurExportOptionWindow_DeletePreset", "Delete Preset"))
					.IsEnabled_Lambda([this]() { return PresetComboBox->GetSelectedItem() != nullptr; })
					.OnClicked(this, &SFurCombModeWidget::DeletePreset)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()				
			.Padding(0.0f, 4.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1)
				.Padding(2.0f, 0.0f)
				[
					SAssignNew(NewPresetName, SEditableTextBox)
					.Text(LOCTEXT("GFurExportOptionWindow_NewPreset", "New Presset"))
				]
				+ SHorizontalBox::Slot()
				.Padding(2.0f, 0.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("GFurExportOptionWindow_SavePreset", "Save Preset"))
					.IsEnabled_Lambda([this]() { return !NewPresetName->GetText().IsEmpty(); })
					.OnClicked(this, &SFurCombModeWidget::SavePreset)
				]
			]
		]
	];

	FurComb->GetCurrentFurCombSettings()->RegWidget(this);
	UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
}

void SFurCombModeWidget::UpdateSelectedPresset(UFurCombSettings* InSettings)
{
	for (int i = 0; i < PresetSettings.Num(); i++)
	{
		if (InSettings->Equals(PresetSettings[i]))
		{
			PresetComboBox->SetSelectedItem(PresetOptions[i]);
			return;
		}
	}
	PresetComboBox->ClearSelection();
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
	SettingsDetailsView->SetIsPropertyVisibleDelegate(FIsPropertyVisible::CreateRaw(this, &SFurCombModeWidget::IsPropertyVisible));
	SettingsDetailsView->SetObjects(SettingsObjects);
}

TSharedPtr<SWidget> SFurCombModeWidget::CreateToolBarWidget()
{
	FToolBarBuilder ModeSwitchButtons(MakeShareable(new FUICommandList()), FMultiBoxCustomization::None);
	{
		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Length);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Length; })),
			NAME_None, LOCTEXT("Mode.FurComb.Length", "Length"), LOCTEXT("Mode.FurComb.Length.Tooltip", "Length - makes the fur longer, holding shift while combing makes the fur shorter"),
			FSlateIcon("gFurStyleSet", "gFur.Length", "gFur.Length.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::AverageLength);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::AverageLength; })),
			NAME_None, LOCTEXT("Mode.FurComb.AverageLength", "Average Length"), LOCTEXT("Mode.FurComb.Average.Tooltip", "Average Length - averages the length of fur in the combed area"),
			FSlateIcon("gFurStyleSet", "gFur.AverageLength", "gFur.AverageLength.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Bend);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Bend; })),
			NAME_None, LOCTEXT("Mode.FurComb.Bend", "Bend"), LOCTEXT("Mode.FurComb.Bend.Tooltip", "Bend - bends the fur along the brush stroke, using shift inverses the effect"),
			FSlateIcon("gFurStyleSet", "gFur.Bend", "gFur.Bend.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Clump);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Clump; })),
			NAME_None, LOCTEXT("Mode.FurComb.Clump", "Clump"), LOCTEXT("Mode.FurComb.Clump.Tooltip", "Clump - clumps the fur along the brush stroke, using shift inverses the effect"),
			FSlateIcon("gFurStyleSet", "gFur.Clump", "gFur.Clump.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Twist);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Twist; })),
			NAME_None, LOCTEXT("Mode.FurComb.Twist", "Twist"), LOCTEXT("Mode.FurComb.Twist.Tooltip", "Twist - twists the fur, shift inverts the effect"),
			FSlateIcon("gFurStyleSet", "gFur.Twist", "gFur.Twist.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Noise);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Noise; })),
			NAME_None, LOCTEXT("Mode.FurComb.Noise", "Noise"), LOCTEXT("Mode.FurComb.Noise.Tooltip", "Noise - adds noise to the fur, shift inverts the effect"),
			FSlateIcon("gFurStyleSet", "gFur.Noise", "gFur.Noise.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]() {
			FurComb->SetMode(EFurCombMode::Curl);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Curl; })),
			NAME_None, LOCTEXT("Mode.FurComb.Curl", "Curl"), LOCTEXT("Mode.FurComb.Curl.Tooltip", "Curl - curls or twists single hair guide along it's pivot"),
			FSlateIcon("gFurStyleSet", "gFur.Curl", "gFur.Curl.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::Relax);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::Relax; })),
			NAME_None, LOCTEXT("Mode.FurComb.Relax", "Relax"), LOCTEXT("Mode.FurComb.Relax.Tooltip", "Relax - relaxes the fur towards being straight"),
			FSlateIcon("gFurStyleSet", "gFur.Relax", "gFur.Relax.Small"), EUserInterfaceActionType::ToggleButton);

		ModeSwitchButtons.AddToolBarButton(FUIAction(FExecuteAction::CreateLambda([=]()
		{
			FurComb->SetMode(EFurCombMode::AddRemove);
			SettingsObjects.Empty();
			SettingsObjects.Add(FurComb->GetCurrentFurCombSettings());
			SettingsDetailsView->SetObjects(SettingsObjects);
			FurComb->GetCurrentFurCombSettings()->RegWidget(this);
			UpdateSelectedPresset(FurComb->GetCurrentFurCombSettings());
		}), FCanExecuteAction(), FIsActionChecked::CreateLambda([=]() -> bool { return FurComb->GetMode() == EFurCombMode::AddRemove; })),
			NAME_None, LOCTEXT("Mode.FurComb.AddRemove", "AddRemove"), LOCTEXT("Mode.FurComb.AddRemove.Tooltip", "Add/Remove - adds fur, shift removes fur completely"),
			FSlateIcon("gFurStyleSet", "gFur.AddRemove", "gFur.AddRemove.Small"), EUserInterfaceActionType::ToggleButton);
	}

	return ModeSwitchButtons.MakeWidget();
}

void SFurCombModeWidget::LoadPresets()
{
	TArray<FString> PresetNames;
	GConfig->GetArray(TEXT("FurCombEdit"), TEXT("CombPresets"), PresetNames, GEditorPerProjectIni);
	for (const auto& PresetName : PresetNames)
	{
		auto PresetNamePtr = MakeShared<FString>(PresetName);
		PresetOptions.Add(PresetNamePtr);

		UFurCombSettings* Settings = NewObject<UFurCombSettings>();
		Settings->AddToRoot();
		Settings->SetConfigPrefix(FString("Preset_") + PresetName);
		Settings->Load();
		PresetSettings.Add(Settings);
	}
}

void SFurCombModeWidget::SelectPreset(const TSharedPtr<FString>& InOption)
{
	if (InOption)
	{
		NewPresetName->SetText(FText::FromString(*InOption));

		for (int32 i = 0; i < PresetOptions.Num(); i++)
		{
			if (!PresetOptions[i]->Compare(*InOption))
			{
				FurComb->GetCurrentFurCombSettings()->CopyFrom(PresetSettings[i]);
				break;
			}
		}
	}
}

FReply SFurCombModeWidget::DeletePreset()
{
	int32 Index = PresetOptions.IndexOfByKey(PresetComboBox->GetSelectedItem());
	if (Index >= 0)
	{
		PresetOptions.RemoveAt(Index);
		PresetComboBox->RefreshOptions();
		PresetComboBox->ClearSelection();

		PresetSettings[Index]->DeleteFromConfig();
		PresetSettings[Index]->ConditionalBeginDestroy();
		PresetSettings.RemoveAt(Index);

		TArray<FString> PresetNames;
		for (const auto& Preset : PresetOptions)
			PresetNames.Add(*Preset);
		GConfig->SetArray(TEXT("FurCombEdit"), TEXT("CombPresets"), PresetNames, GEditorPerProjectIni);
	}
	return FReply::Handled();
}

FReply SFurCombModeWidget::SavePreset()
{
	FString PresetName = NewPresetName->GetText().ToString();
	for (int32 i = 0; i < PresetOptions.Num(); i++)
	{
		if (!PresetOptions[i]->Compare(PresetName))
		{
			PresetSettings[i]->CopyFrom(FurComb->GetCurrentFurCombSettings());
			return FReply::Handled();
		}
	}

	auto PresetNamePtr = MakeShared<FString>(PresetName);
	PresetOptions.Add(PresetNamePtr);
	PresetComboBox->RefreshOptions();
	PresetComboBox->SetSelectedItem(PresetNamePtr);

	UFurCombSettings* Settings = NewObject<UFurCombSettings>();
	Settings->AddToRoot();
	Settings->SetConfigPrefix(FString("Preset_") + PresetName);
	Settings->CopyFrom(FurComb->GetCurrentFurCombSettings());
	PresetSettings.Add(Settings);

	TArray<FString> PresetNames;
	for (const auto& Preset : PresetOptions)
		PresetNames.Add(*Preset);
	GConfig->SetArray(TEXT("FurCombEdit"), TEXT("CombPresets"), PresetNames, GEditorPerProjectIni);

	return FReply::Handled();
}

bool SFurCombModeWidget::IsPropertyVisible(const FPropertyAndParent& p)
{
	return FurComb->GetMode() == EFurCombMode::Curl || p.Property.GetNameCPP() != "TwistCount";
}

#undef LOCTEXT_NAMESPACE // "PaintModePainter"
