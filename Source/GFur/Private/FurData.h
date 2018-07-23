
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

	virtual void UpdateSkeletonShaderData(float FurOffsetPower, const TArray<FMatrix>& ReferenceToLocal, const TArray<FVector>& LinearOffsets, const TArray<FVector>& AngularOffsets,
		const TArray<FMatrix>& Transformations, const TArray<FBoneIndexType>& BoneMap, uint32 FrameNumber, ERHIFeatureLevel::Type FeatureLevel) {}
	virtual void UpdateStaticShaderData(float FurOffsetPower, const FVector& LinearOffset, const FVector& AngularOffset,
		const FVector& Position, uint32 FrameNumber, ERHIFeatureLevel::Type FeatureLevel) {}
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

	virtual ~FFurData()																	{}
	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, ERHIFeatureLevel::Type InFeatureLevel) = 0;
};
