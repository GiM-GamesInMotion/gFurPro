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

void SetGeometry(Alembic::AbcGeom::OCurvesSchema& Schema, const FVector* Points, int32 NumPoints, int32 NumSplines, int32 NumControlPoints)
{
	TArray<int32> NumVerticesData;
	NumVerticesData.AddUninitialized(NumSplines);
	for (int32 i = 0; i < NumSplines; i++)
		NumVerticesData[i] = NumControlPoints;

	Alembic::Abc::P3fArraySample Positions((Alembic::Abc::P3fArraySample::value_type*)Points, NumPoints);
	Alembic::Abc::Int32ArraySample NumVertices((Alembic::Abc::Int32ArraySample::value_type*) & NumVerticesData[0], NumVerticesData.Num());
	Alembic::AbcGeom::OCurvesSchema::Sample Sample(Positions, NumVertices);

	Schema.set(Sample);
}

void SetGroomProperties(Alembic::AbcGeom::OCurvesSchema& Schema)
{
	Alembic::Abc::OCompoundProperty Properties = Schema.getArbGeomParams();

	Alembic::AbcGeom::OBoolGeomParam Params(Properties, "groom_guide", false, Alembic::AbcGeom::kConstantScope, 1);

	Alembic::Abc::BoolArraySample::value_type value = true;
	Alembic::Abc::BoolArraySample GroomGuide(&value, 1);

	Alembic::AbcGeom::OBoolGeomParam::Sample Sample(GroomGuide, Alembic::AbcGeom::kConstantScope);

	Params.set(Sample);
}

void ExportFurSplines(const FString& filename, const TArray<FVector>& Points, int32 ControlPointCount, int32 GroomSplineCount)
{
	std::string path(TCHAR_TO_UTF8(*filename));

	Alembic::Abc::OArchive Archive(Alembic::AbcCoreHDF5::WriteArchive(), path);

	Alembic::AbcGeom::OCurves GroomCurves(Archive.getTop(), "groom_splines");
	Alembic::AbcGeom::OCurvesSchema& GroomSchema = GroomCurves.getSchema();

	SetGeometry(GroomSchema, &Points[0], GroomSplineCount * ControlPointCount, GroomSplineCount, ControlPointCount);

	int32 NumSplines = Points.Num() / ControlPointCount;
	int32 NumStrandSplines = NumSplines - GroomSplineCount;

	Alembic::AbcGeom::OCurves StrandCurves(Archive.getTop(), "strand_splines");
	Alembic::AbcGeom::OCurvesSchema& StrandSchema = StrandCurves.getSchema();

	SetGeometry(StrandSchema, &Points[GroomSplineCount * ControlPointCount], NumStrandSplines * ControlPointCount, NumStrandSplines, ControlPointCount);
}
