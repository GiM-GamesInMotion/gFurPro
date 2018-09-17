// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/RenderCore/Public/RenderResource.h"
#include "VertexFactory.h"
#include "BoneIndices.h"
#include "FurSplines.h"

/** Index Buffer */
class FFurIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI() override;
};

/** Vertex Factory */
class FFurVertexFactory : public FVertexFactory
{
public:
	FFurVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FVertexFactory(InFeatureLevel)
	{
	}

	virtual void UpdateSkeletonShaderData(float InFurOffsetPower, const TArray<FMatrix>& InReferenceToLocal, const TArray<FVector>& InLinearOffsets,
		const TArray<FVector>& InAngularOffsets, const TArray<FMatrix>& InTransformations, const TArray<FBoneIndexType>& InBoneMap, uint32 InFrameNumber,
		ERHIFeatureLevel::Type InFeatureLevel) {}
	virtual void UpdateStaticShaderData(float InFurOffsetPower, const FVector& InLinearOffset, const FVector& InAngularOffset,
		const FVector& InPosition, uint32 InFrameNumber, ERHIFeatureLevel::Type InFeatureLevel) {}
};

/** Fur Data */
struct FFurData
{
	struct FSection
	{
		uint32 MaterialIndex;
		uint32 BaseIndex;
		uint32 NumTriangles;
		uint32 MinVertexIndex;
		uint32 MaxVertexIndex;
		int32 NumBones;
	};

	UFurSplines* FurSplines;
	FFurIndexBuffer IndexBuffer;
	TArray<FSection> Sections;
	int Lod;
	int FurLayerCount;
	float FurLength;
	float ShellBias;
	float HairLengthForceUniformity;
	float MinFurLength;
	float CurrentMinFurLength;
	float CurrentMaxFurLength;
	float MaxVertexBoneDistance;
	float NoiseStrength;
	int RefCount = 0;
	bool RemoveFacesWithoutSplines;

	virtual ~FFurData()																	{}
	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel) = 0;
};
