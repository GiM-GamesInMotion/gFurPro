// Copyright 2020 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "Editor/UnrealEd/Public/EditorReimportHandler.h"
#include "FurSplineAbcImporterFactory.generated.h"

class UFurSplines;

class SGFurImportOptions : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGFurImportOptions)
		: _WidgetWindow()
		{}

		SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_ARGUMENT(FText, FullPath)
	SLATE_END_ARGS()

	TOptional<int> GetConversionType() const
	{
		return ConversionType;
	}

	TOptional<float> GetThreshold() const
	{
		return Threshold;
	}

public:
	void Construct(const FArguments& InArgs);
	virtual bool SupportsKeyboardFocus() const override { return true; }

	FReply OnImport()
	{
		bShouldImport = true;
		if (WidgetWindow.IsValid())
		{
			WidgetWindow.Pin()->RequestDestroyWindow();
		}
		return FReply::Handled();
	}

	FReply OnCancel()
	{
		bShouldImport = false;
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

	bool ShouldImport() const
	{
		return bShouldImport;
	}


	SGFurImportOptions()
	: bShouldImport(false)
	{}

private:
	bool CanImport() const
	{
		return true;
	}

private:
	TWeakPtr< SWindow > WidgetWindow;
	TSharedPtr< SButton > ImportButton;
	bool			bShouldImport;
	TSharedPtr<IDetailsView> DetailsView;
	TArray<TSharedPtr<int>> PresetOptions;
	int ConversionType = 0;
	float Threshold = 0.1f;

	FText TextOption(int InOption);
};


UCLASS()
class UFurSplineAbcImporterFactory : public UFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	// End of UFactory interface

	// FReimportHandler interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	// End of FReimportHandler interface

protected:

	static UObject* CreateNewAsset(UClass* AssetClass, const FString& TargetPath, const FString& DesiredName, EObjectFlags Flags);

private:
	void ShowImportOptionsWindow(TSharedPtr<SGFurImportOptions>& Options, FString FilePath) const;
};
