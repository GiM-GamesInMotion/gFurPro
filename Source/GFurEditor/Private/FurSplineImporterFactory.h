// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Factories/Factory.h"
#include "Editor/UnrealEd/Public/EditorReimportHandler.h"
#include "FurSplineImporterFactory.generated.h"

namespace fbxsdk
{
	class FbxNode;
	class FbxManager;
	class FbxScene;
};

class UFurSplines;

UCLASS()
class UFurSplineImporterFactory : public UFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

	// UFactory interface
	virtual FText GetToolTip() const override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn) override;
	// End of UFactory interface

	// FReimportHandler interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	virtual int32 GetPriority() const override;
	// End of FReimportHandler interface

protected:

	static UObject* CreateNewAsset(UClass* AssetClass, const FString& TargetPath, const FString& DesiredName, EObjectFlags Flags);

	fbxsdk::FbxScene* ImportFbxScene(fbxsdk::FbxManager* SdkManager, const FString& Filename);
	fbxsdk::FbxScene* ImportFbxScene(fbxsdk::FbxManager* SdkManager, const char* Buffer, intptr_t Size);
	bool ImportSplines(fbxsdk::FbxScene* Scene, UFurSplines* Splines);
	bool CheckNode(fbxsdk::FbxNode* Node, bool& hasSpline);
	bool ImportNode(fbxsdk::FbxNode* Node, UFurSplines* FurSplines);
	bool ImportLine(fbxsdk::FbxNode* Node, UFurSplines* FurSplines);
};
