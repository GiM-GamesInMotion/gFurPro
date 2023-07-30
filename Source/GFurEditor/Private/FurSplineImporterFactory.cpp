// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurSplineImporterFactory.h"
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

#include <fbxsdk.h>
#include <fbxsdk/core/fbxsystemunit.h>
#include <fbxsdk/fileio/fbxiosettings.h>
#include <fbxsdk/fileio/fbximporter.h>
#include <fbxsdk/utils/fbxgeometryconverter.h>

class MemoryStream : public FbxStream
{
public:
	inline MemoryStream(const void* InData, FbxInt64 InSize, int InReaderId)
		: Data(InData), Size(InSize), Position(0), State(EState::eClosed), ReaderId(InReaderId)
	{

	}

	// Inherited via FbxStream
	virtual EState GetState() override { return State; }
	virtual bool Open(void* pStreamData) override { State = EState::eOpen; Position = 0; return true; }
	virtual bool Close() override { State = EState::eClosed; return true; }
	virtual bool Flush() override { return true; }
	virtual int Write(const void *, int) override { return false; }
	virtual int Read(void* data, int size) const override
	{
		if (Position + size > Size)
		{
			ErrorCode = 1;
			return 0;
		}
		memcpy(data, (const char*)Data + Position, size);
		Position += size;
		return size;
	}
	virtual int GetReaderID() const override { return ReaderId; }
	virtual int GetWriterID() const override { return -1; }
	virtual void Seek(const FbxInt64 &pOffset, const FbxFile::ESeekPos &pSeekPos) override
	{
		switch (pSeekPos)
		{
		case FbxFile::ESeekPos::eCurrent:
			Position += pOffset;
			break;
		case FbxFile::ESeekPos::eEnd:
			Position = Size - pOffset;
			break;
		default:
			Position = pOffset;
			break;
		}
	}
	virtual long GetPosition() const override { return Position; }
	virtual void SetPosition(long pPosition) override { Position = pPosition; }
	virtual int GetError() const override { return ErrorCode; }
	virtual void ClearError() override { ErrorCode = 0; }

private:
	const void* Data;
	FbxInt64 Size;
	mutable FbxInt64 Position;
	EState State;
	mutable int ErrorCode;
	int ReaderId;
};


UFurSplineImporterFactory::UFurSplineImporterFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	//bEditAfterNew = true;
	SupportedClass = UFurSplines::StaticClass();

	bEditorImport = true;
	bText = false;
//	AutoPriority = 0x7fffffff;
//	ImportPriority = 0x7fffffff;

	Formats.Add(TEXT("fbx;FBX fur splines"));
}

FText UFurSplineImporterFactory::GetToolTip() const
{
	return FText::FromString("Splines for fur");
}

bool UFurSplineImporterFactory::FactoryCanImport(const FString& Filename)
{
	FbxManager* SdkManager = FbxManager::Create();
	check(SdkManager);
	FbxScene* Scene = ImportFbxScene(SdkManager, Filename);
	if (Scene == NULL)
	{
		SdkManager->Destroy();
		return false;
	}

	bool hasSpline = false;
	FbxNode* root = Scene->GetRootNode();
	if (root)
	{
		for (int i = 0; i < root->GetChildCount(); i++)
		{
			FbxNode* node = root->GetChild(i);
			if (!CheckNode(node, hasSpline))
			{
				SdkManager->Destroy();
				return false;
			}
			if (hasSpline)
				break;
		}
	}

	SdkManager->Destroy();
	return hasSpline;
}

UObject* UFurSplineImporterFactory::FactoryCreateBinary(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const uint8*& Buffer, const uint8* BufferEnd, FFeedbackContext* Warn)
{
	FbxManager* SdkManager = FbxManager::Create();
	check(SdkManager);
	FbxScene* Scene = ImportFbxScene(SdkManager, (const char*)Buffer, BufferEnd - Buffer);
	if (Scene == NULL)
	{
		SdkManager->Destroy();
		return NULL;
	}

	UFurSplines* Result = NewObject<UFurSplines>(InParent, InName, Flags);
	if (!ImportSplines(Scene, Result))
	{
		delete Result;
		SdkManager->Destroy();
		return nullptr;
	}
	Result->ImportFilename = GetCurrentFilename();
	SdkManager->Destroy();
	return Result;
}

bool UFurSplineImporterFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
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

void UFurSplineImporterFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
	if (UFurSplines* Splines = Cast<UFurSplines>(Obj))
	{
		if (ensure(NewReimportPaths.Num() == 1))
		{
			Splines->ImportFilename = NewReimportPaths[0];
		}
	}
}

EReimportResult::Type UFurSplineImporterFactory::Reimport(UObject* Obj)
{
	if (UFurSplines* Splines = Cast<UFurSplines>(Obj))
	{
		FbxManager* sdkManager = FbxManager::Create();
		check(sdkManager);
		FbxScene* Scene = ImportFbxScene(sdkManager, Splines->ImportFilename);
		if (Scene == NULL)
		{
			sdkManager->Destroy();
			return EReimportResult::Failed;
		}

		Splines->Modify();
		Splines->Vertices.Reset();
		Splines->Index.Reset();
		Splines->Count.Reset();
		Splines->Version = 1;
		EReimportResult::Type r = ImportSplines(Scene, Splines) ? EReimportResult::Succeeded : EReimportResult::Failed;
		Splines->UpdateSplines();
		sdkManager->Destroy();
		Splines->OnSplinesChanged.Broadcast();
		return r;
	}
	return EReimportResult::Failed;
}

int32 UFurSplineImporterFactory::GetPriority() const
{
	return ImportPriority;
}

FbxScene* UFurSplineImporterFactory::ImportFbxScene(FbxManager* SdkManager, const FString& Filename)
{
	FbxIOSettings* ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(ios);
	FbxScene* Scene = FbxScene::Create(SdkManager, "My Scene");
	check(Scene);

	FbxImporter* importer = FbxImporter::Create(SdkManager, "");
	if (!importer->Initialize(TCHAR_TO_ANSI(*Filename), -1, SdkManager->GetIOSettings()))
		return NULL;

	if (!importer->Import(Scene))
	{
		importer->Destroy();
		return NULL;
	}
	importer->Destroy();
	return Scene;
}

FbxScene* UFurSplineImporterFactory::ImportFbxScene(FbxManager* SdkManager, const char* Buffer, intptr_t Size)
{
	FbxIOSettings* ios = FbxIOSettings::Create(SdkManager, IOSROOT);
	SdkManager->SetIOSettings(ios);
	FbxScene* Scene = FbxScene::Create(SdkManager, "My Scene");
	check(Scene);

	FbxImporter* importer = FbxImporter::Create(SdkManager, "");
	FbxIOPluginRegistry* r = SdkManager->GetIOPluginRegistry();
	MemoryStream stream(Buffer, Size, r->FindReaderIDByExtension("fbx"));
	if (!importer->Initialize(&stream, NULL, -1, SdkManager->GetIOSettings()))
		return NULL;

	if (!importer->Import(Scene))
	{
		importer->Destroy();
		return NULL;
	}
	importer->Destroy();
	return Scene;
}

bool UFurSplineImporterFactory::ImportSplines(FbxScene* Scene, UFurSplines* Splines)
{
	/*
	FbxAxisSystem::ECoordSystem CoordSystem = FbxAxisSystem::eRightHanded;
	FbxAxisSystem::EUpVector UpVector = FbxAxisSystem::eZAxis;
	FbxAxisSystem::EFrontVector FrontVector = (FbxAxisSystem::EFrontVector) - FbxAxisSystem::eParityOdd;
	//	if (GetImportOptions()->bForceFrontXAxis)
	{
	FrontVector = FbxAxisSystem::eParityEven;
	}


	FbxAxisSystem UnrealImportAxis(UpVector, FrontVector, CoordSystem);

	FbxAxisSystem SourceSetup = Scene->GetGlobalSettings().GetAxisSystem();

	if (SourceSetup != UnrealImportAxis)
	{
	FbxRootNodeUtility::RemoveAllFbxRoots(Scene);
	UnrealImportAxis.ConvertScene(Scene);
	FbxAMatrix JointOrientationMatrix;
	JointOrientationMatrix.SetIdentity();
	//		if (GetImportOptions()->bForceFrontXAxis)
	{
	JointOrientationMatrix.SetR(FbxVector4(-90.0, -90.0, 0.0));
	}
	//		FFbxDataConverter::SetJointPostConversionMatrix(JointOrientationMatrix);
	}
	FbxSystemUnit::cm.ConvertScene(Scene);
	*/

	FbxNode* root = Scene->GetRootNode();
	if (!root)
		return false;

	for (int i = 0; i < root->GetChildCount(); i++)
	{
		FbxNode* node = root->GetChild(i);
		if (!ImportNode(node, Splines))
		{
			return false;
		}
	}
	return true;
}

bool UFurSplineImporterFactory::CheckNode(fbxsdk::FbxNode* Node, bool& hasSpline)
{
	if (Node->GetNodeAttribute() == NULL)
		return false;

	FbxNodeAttribute::EType lAttributeType = (Node->GetNodeAttribute()->GetAttributeType());
	switch (lAttributeType)
	{
	case FbxNodeAttribute::eLine:
		hasSpline = true;
		return true;
	default:
		break;
	}

	for (int i = 0; i < Node->GetChildCount(); i++)
	{
		if (!CheckNode(Node->GetChild(i), hasSpline))
			return false;
		if (hasSpline)
			return true;
	}
	return true;
}


bool UFurSplineImporterFactory::ImportNode(FbxNode* Node, UFurSplines* Splines)
{
	if (Node->GetNodeAttribute() == NULL)
		return false;

	FbxNodeAttribute::EType lAttributeType = (Node->GetNodeAttribute()->GetAttributeType());
	switch (lAttributeType)
	{
	case FbxNodeAttribute::eLine:
		if (!ImportLine(Node, Splines))
			return false;
		break;
		//	case FbxNodeAttribute::eSkeleton:
		//			DisplaySkeleton(node);
		//		break;
	default:
		break;
	}

	for (int i = 0; i < Node->GetChildCount(); i++)
	{
		if (!ImportNode(Node->GetChild(i), Splines))
			return false;
	}
	return true;
}

bool UFurSplineImporterFactory::ImportLine(FbxNode* Node, UFurSplines* FurSplines)
{
	FbxLine* Line = (FbxLine*)Node->GetNodeAttribute();

	FbxVector4* controlPoints = Line->GetControlPoints();

	FbxArray<int>* Indices = Line->GetIndexArray();
	if (Indices->Size() == 0)
		return true;
	FbxArray<int>* EndPoints = Line->GetEndPointArray();

	int NextEndPoint = 0;
	int NextEndPointIndex = EndPoints->GetAt(NextEndPoint);
	int StartIndex = 0;
	for (int i = 0; i < Indices->Size(); i++)
	{
		int index = Indices->GetAt(i);

		const FbxVector4& v = controlPoints[index];
		FurSplines->Vertices.Add(FVector(v.mData[0], -v.mData[1], v.mData[2]));
		if (NextEndPointIndex == i)
		{
			FurSplines->Index.Add(StartIndex);
			FurSplines->Count.Add(FurSplines->Vertices.Num() - StartIndex);
			StartIndex = FurSplines->Vertices.Num();

			NextEndPoint++;
			if (NextEndPoint < EndPoints->Size())
				NextEndPointIndex = EndPoints->GetAt(NextEndPoint);
		}
	}
	FurSplines->Version = 1;

	return true;
}

