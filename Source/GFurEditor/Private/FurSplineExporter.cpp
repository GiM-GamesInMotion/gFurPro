// Copyright 2020 GiM s.r.o. All Rights Reserved.

#include "FurSplineExporter.h"
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

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif

PRAGMA_DEFAULT_VISIBILITY_START
THIRD_PARTY_INCLUDES_START
#include <Alembic/AbcGeom/All.h>
#include <Alembic/Abc/OArchive.h>
#include <Alembic/Abc/OObject.h>
#include <Alembic/AbcCoreHDF5/All.h>
THIRD_PARTY_INCLUDES_END
PRAGMA_DEFAULT_VISIBILITY_END

#if PLATFORM_WINDOWS
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void ExportFurSplines(UFurSplines* FurSplines, const FString& filename)
{
	std::string path(TCHAR_TO_UTF8(*filename));

	Alembic::Abc::OArchive Archive(Alembic::AbcCoreHDF5::WriteArchive(), path);

	Alembic::AbcGeom::OCurves Curves(Archive.getTop(), "splines");
	Alembic::AbcGeom::OCurvesSchema& Schema = Curves.getSchema();

	int32 ControlPointCount = FurSplines->ControlPointCount;
	int32 NumSplines = FurSplines->Vertices.Num() / ControlPointCount;
	TArray<int32> NumVerticesData;
	NumVerticesData.AddUninitialized(NumSplines);
	for (int32 i = 0; i < NumSplines; i++)
		NumVerticesData[i] = ControlPointCount;

	Alembic::Abc::P3fArraySample Positions((Alembic::Abc::P3fArraySample::value_type*)&FurSplines->Vertices[0], FurSplines->Vertices.Num());
	Alembic::Abc::Int32ArraySample NumVertices((Alembic::Abc::Int32ArraySample::value_type*)&NumVerticesData[0], NumVerticesData.Num());
	Alembic::AbcGeom::OCurvesSchema::Sample Sample(Positions, NumVertices);

	Schema.set(Sample);
}
