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

void SetGeometry(Alembic::AbcGeom::OCurvesSchema& Schema, const FVector* Points, const FVector2D* UVs, int32 NumPoints, int32 NumSplines, int32 NumControlPoints)
{
	TArray<int32> NumVerticesData;
	NumVerticesData.AddUninitialized(NumSplines);
	for (int32 i = 0; i < NumSplines; i++)
		NumVerticesData[i] = NumControlPoints;

	Alembic::Abc::P3fArraySample Positions((Alembic::Abc::P3fArraySample::value_type*)Points, NumPoints);
	Alembic::Abc::Int32ArraySample NumVertices((Alembic::Abc::Int32ArraySample::value_type*)&NumVerticesData[0], NumVerticesData.Num());
	Alembic::Abc::V2fArraySample UVsArray((Alembic::AbcGeom::OV2fGeomParam::value_type*)UVs, NumSplines);
	Alembic::AbcGeom::OCurvesSchema::Sample Sample(Positions, NumVertices);

	Alembic::AbcGeom::OV2fGeomParam::Sample UVsSample(UVsArray, Alembic::AbcGeom::GeometryScope::kUniformScope);

	Schema.set(Sample);

	Alembic::AbcGeom::OV2fGeomParam param(Schema.getArbGeomParams(), "groom_root_uv", false, Alembic::AbcGeom::GeometryScope::kUniformScope,
		NumSplines);
	param.set(UVsSample);
}

void SetGroomProperties(Alembic::AbcGeom::OCurvesSchema& Schema)
{
	Alembic::Abc::OCompoundProperty Properties = Schema.getArbGeomParams();

	Alembic::AbcGeom::OInt16GeomParam Params(Properties, "groom_guide", false, Alembic::AbcGeom::kConstantScope, 1);

	Alembic::Abc::Int16ArraySample::value_type value = 1;
	Alembic::Abc::Int16ArraySample GroomGuide(&value, 1);

	Alembic::AbcGeom::OInt16GeomParam::Sample Sample(GroomGuide, Alembic::AbcGeom::kConstantScope);

	Params.set(Sample);
}

void ExportFurSplines(const FString& filename, const TArray<FVector>& Points, const TArray<FVector2D>& UVs, int32 ControlPointCount, int32 GroomSplineCount)
{
	TArray<FVector2D> UVs2;
	UVs2.AddUninitialized(UVs.Num());
	for (int i = 0; i < UVs.Num(); i++)
	{
		UVs2[i].X = UVs[i].X;
		UVs2[i].Y = 1.0f - UVs[i].Y;
	}

	{
		std::string path(TCHAR_TO_UTF8(*filename));

		Alembic::Abc::OArchive Archive(Alembic::AbcCoreHDF5::WriteArchive(), path);

		Alembic::AbcGeom::OCurves GroomCurves(Archive.getTop(), "groom_splines");
		Alembic::AbcGeom::OCurvesSchema& GroomSchema = GroomCurves.getSchema();

		SetGeometry(GroomSchema, &Points[0], &UVs2[0], GroomSplineCount * ControlPointCount, GroomSplineCount, ControlPointCount);
		SetGroomProperties(GroomSchema);

		int32 NumSplines = Points.Num() / ControlPointCount;
		int32 NumStrandSplines = NumSplines - GroomSplineCount;

		Alembic::AbcGeom::OCurves StrandCurves(Archive.getTop(), "strand_splines");
		Alembic::AbcGeom::OCurvesSchema& StrandSchema = StrandCurves.getSchema();

		SetGeometry(StrandSchema, &Points[GroomSplineCount * ControlPointCount], &UVs2[GroomSplineCount],
			NumStrandSplines * ControlPointCount, NumStrandSplines, ControlPointCount);
	}
}
