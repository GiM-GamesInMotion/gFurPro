// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/RenderCore/Public/RenderResource.h"
#include "Runtime/Engine/Public/Rendering/StaticMeshVertexBuffer.h"
#include "Runtime/Engine/Public/Rendering/PositionVertexBuffer.h"
#include "Runtime/Engine/Public/Rendering/ColorVertexBuffer.h"
#include "VertexFactory.h"
#include "BoneIndices.h"
#include "FurSplines.h"

/** Fur Static Vertex */
template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
struct FFurStaticVertex
{
	typedef typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT TangentTypeT;
	typedef typename TStaticMeshVertexUVsTypeSelector<UVTypeT>::UVsTypeT UVsTypeT;

	FVector			Position;

	// Tangent, U-direction
	TangentTypeT	TangentX;
	// Binormal, V-direction
//	TangentTypeT	TangentY;
	// Normal
	TangentTypeT	TangentZ;

	// UVs
	UVsTypeT		UV0;
	// UVs
	FVector2D		UV1;
	// UVs
	FVector2D		UV2;
	// UVs
	FVector2D		UV3;
	// VertexColor
	FColor			Color;

	FVector			FurOffset;
};

/** Fur Vertex Buffer */
class FFurVertexBuffer : public FVertexBuffer
{
public:
	virtual ~FFurVertexBuffer() override;
	virtual void InitRHI() override;

	template<typename VertexType>
	VertexType* Lock(uint32 VertexCount);
	void Unlock();

	uint32 GetSize() const { return Size; }
	uint32 GetVertexSize() const { return VertexSize; }

private:
	uint8* VertexData = nullptr;
	uint32 Size = 0;
	uint32 VertexSize = 0;
};

template<typename VertexType>
VertexType* FFurVertexBuffer::Lock(uint32 InVertexCount)
{
	if (InVertexCount == 0)
		InVertexCount = 1;
	VertexSize = sizeof(VertexType);
	uint32 NewSize = InVertexCount * sizeof(VertexType);
	if (NewSize != Size)
	{
		delete[] VertexData;
		VertexData = new uint8[NewSize];
		Size = NewSize;
	}
	return (VertexType*)VertexData;
}


/** Index Buffer */
class FFurIndexBuffer : public FIndexBuffer
{
public:
	virtual void InitRHI() override;

	TArray<int32>& Lock();
	void Unlock();

private:
	TArray<int32> Indices;
};

/** Vertex Factory */
class FFurVertexFactory : public FVertexFactory
{
public:
	FFurVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FVertexFactory(InFeatureLevel)
	{
	}

	virtual void UpdateSkeletonShaderData(float InFurOffsetPower, float InMaxPhysicsOffsetLength, const TArray<FMatrix>& InReferenceToLocal, const TArray<FVector>& InLinearOffsets,
		const TArray<FVector>& InAngularOffsets, const TArray<FMatrix>& InTransformations, const TArray<FBoneIndexType>& InBoneMap, bool InDiscontinuous,
		ERHIFeatureLevel::Type InFeatureLevel) {}
	virtual void UpdateStaticShaderData(float InFurOffsetPower, const FVector& InLinearOffset, const FVector& InAngularOffset,
		const FVector& InPosition, bool InDiscontinuous, ERHIFeatureLevel::Type InFeatureLevel) {}
};

/** Fur Data */
class FFurData
{
public:
	struct FSection
	{
		uint32 MaterialIndex;
		uint32 BaseIndex;
		uint32 NumTriangles;
		uint32 MinVertexIndex;
		uint32 MaxVertexIndex;
		int32 NumBones;
	};

	static const int32 MinimalFurLayerCount;
	static const int32 MaximalFurLayerCount;
	static const float MinimalFurLength;

	const TArray<FSection>& GetSections_RenderThread() const { check(IsInRenderingThread()); return Sections; }
	int32 GetNumVertices_RenderThread() const { check(IsInRenderingThread()); return VertexCount; }
	const FIndexBuffer* GetIndexBuffer_RenderThread() const { check(IsInRenderingThread()); return &IndexBuffer; }
	int32 GetLod() const { return Lod; }
	float GetCurrentMinFurLength() const { return CurrentMinFurLength; }
	float GetCurrentMaxFurLength() const { return CurrentMaxFurLength; }
	float GetMaxVertexBoneDistance() const { return MaxVertexBoneDistance; }
	int32 GetFurLayerCount() const { return FurLayerCount; }

	const TArray<int32>& GetSplineMap() const { return SplineMap; }
	const TArray<FVector>& GetVertexNormals() const { return Normals; }

	const TArray<FSection>& GetSections() const { return Sections; }
	FFurVertexBuffer& GetVertexBuffer() { return VertexBuffer; }
	FFurIndexBuffer& GetIndexBuffer() { return IndexBuffer; }

	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel) = 0;

protected:
	enum class BuildType
	{
		Minimal,
		Splines,
		Full,
	};

	struct FFurGenLayerData
	{
		float LinearFactor;
		float NonLinearFactor;
		float LayerNoiseStrength;
	};

	int32 RefCount;

	// set
	UFurSplines* FurSplinesAssigned = nullptr;
	int32 Lod;
	int32 FurLayerCount;
	float FurLength;
	float ShellBias;
	float HairLengthForceUniformity;
	float MinFurLength;
	float NoiseStrength;
	bool RemoveFacesWithoutSplines;

	// generated
	UFurSplines* FurSplinesUsed = nullptr;
	FFurVertexBuffer VertexBuffer;
	FFurIndexBuffer IndexBuffer;
	TArray<FSection> Sections;
	float CurrentMinFurLength;
	float CurrentMaxFurLength;
	float MaxVertexBoneDistance;
	bool bUseHighPrecisionTangentBasis;
	bool bUseFullPrecisionUVs;
	uint32 VertexCount;

	UFurSplines* FurSplinesGenerated = nullptr;

	// Temp Data
	uint32 VertexCountPerLayer;
	TArray<FSection> TempSections;
	TArray<FVector> Normals;
	TArray<int32> SplineMap;
	TArray<uint32> VertexRemap;
	int32 OldFurLayerCount = 0;
	bool OldRemoveFacesWithoutSplines = false;

	volatile bool RenderThreadDataSubmissionPending = false;

	FFurData();
	virtual ~FFurData();

	void Set(int InFurLayerCount, int InLod, class UGFurComponent* InFurComponent);

	bool Compare(int InFurLayerCount, int InLod, class UGFurComponent* InFurComponent);
	bool Similar(int InLod, class UGFurComponent* InFurComponent);

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	void UnpackNormals(const FStaticMeshVertexBuffer& InVertices);
	void GenerateSplineMap(const FPositionVertexBuffer& InPositions);

	FFurGenLayerData CalcFurGenLayerData(int32 Layer);
	void GenerateFurLengths(TArray<float>& FurLengths);
	void GenerateFurVertex(FVector& OutFurOffset, FVector2D& OutUv1, FVector2D& OutUv2, FVector2D& OutUv3, const FVector& InTangentZ, float FurLength, const FFurGenLayerData& InGenLayerData);
	void GenerateFurVertex(FVector& OutFurOffset, FVector2D& OutUv1, FVector2D& OutUv2, FVector2D& OutUv3, const FVector& InTangentZ, float FurLength, const FFurGenLayerData& InGenLayerData, int32 InSplineIndex);

	template<typename VertexTypeT, typename VertexBlitterT>
	uint32 GenerateFurVertices(uint32 SrcVertexIndexBegin, uint32 SrcVertexIndexEnd, VertexTypeT* Vertices, const VertexBlitterT& VertexBlitter);
};

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
void FFurData::UnpackNormals(const FStaticMeshVertexBuffer& InVertices)
{
	typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT> TangentType;
	const TangentType* SrcTangents = reinterpret_cast<const TangentType*>(const_cast<FStaticMeshVertexBuffer&>(InVertices).GetTangentData());
	uint32 NumVertices = InVertices.GetNumVertices();
	Normals.Reset(0);
	Normals.AddUninitialized(NumVertices);
	for (uint32 i = 0; i < NumVertices; i++)
	{
		Normals[i] = SrcTangents[i].TangentZ.ToFVector();
	}
}

template<typename VertexTypeT, typename VertexBlitterT>
inline uint32 FFurData::GenerateFurVertices(uint32 SrcVertexIndexBegin, uint32 SrcVertexIndexEnd, VertexTypeT* Vertices, const VertexBlitterT& VertexBlitter)
{
	if (Vertices == nullptr)
	{
		return 0;
	}

	TArray<float> FurLengths;
	GenerateFurLengths(FurLengths);

	uint32 VerticesPerLayer;
	uint32 DstVertexIndex = 0;
	{
		auto GenLayerData = CalcFurGenLayerData(FurLayerCount);
		for (uint32 SrcVertexIndex = SrcVertexIndexBegin; SrcVertexIndex < SrcVertexIndexEnd; SrcVertexIndex++)
		{
			if (FurSplinesUsed)
			{
				int32 SplineIndex = SplineMap[SrcVertexIndex];
				if (RemoveFacesWithoutSplines)
				{
					if (SplineIndex == -1)
					{
						VertexRemap[SrcVertexIndex] = -1;
						continue;
					}
					else
					{
						VertexRemap[SrcVertexIndex] = DstVertexIndex;
					}
				}
				auto& Vertex = Vertices[DstVertexIndex];
				VertexBlitter.Blit(Vertex, SrcVertexIndex);
				float Length = SplineIndex >= 0 ? FurLengths[SplineIndex] : FurLength;
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], Length, GenLayerData, SplineIndex);
			}
			else
			{
				auto& Vertex = Vertices[DstVertexIndex];
				VertexBlitter.Blit(Vertex, SrcVertexIndex);
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], FurLength, GenLayerData);
			}
			DstVertexIndex++;
		}
	}
	VerticesPerLayer = DstVertexIndex;

	for (int32 Layer = FurLayerCount - 2; Layer >= 0; --Layer)
	{
		auto GenLayerData = CalcFurGenLayerData(Layer + 1);
		for (uint32 SrcVertexIndex = SrcVertexIndexBegin; SrcVertexIndex < SrcVertexIndexEnd; SrcVertexIndex++)
		{
			if (FurSplinesUsed)
			{
				int32 SplineIndex = SplineMap[SrcVertexIndex];
				if (RemoveFacesWithoutSplines)
				{
					if (SplineIndex == -1)
					{
						continue;
					}
				}
				auto& Vertex = Vertices[DstVertexIndex];
				VertexBlitter.Blit(Vertex, SrcVertexIndex);
				float Length = SplineIndex >= 0 ? FurLengths[SplineIndex] : FurLength;
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], Length, GenLayerData, SplineIndex);
			}
			else
			{
				auto& Vertex = Vertices[DstVertexIndex];
				VertexBlitter.Blit(Vertex, SrcVertexIndex);
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], FurLength, GenLayerData);
			}
			DstVertexIndex++;
		}
	}
	return VerticesPerLayer;
}

/** Fur Static Vertex Blitter */
template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
class FFurStaticVertexBlitter
{
public:
	typedef TStaticMeshVertexTangentDatum<typename TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::TangentTypeT> TangentType;
	typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<UVTypeT>::UVsTypeT> UVType;

	FFurStaticVertexBlitter(const FPositionVertexBuffer& InPositions, const FStaticMeshVertexBuffer& InVertices, const FColorVertexBuffer& InColors)
	{
		Positions = reinterpret_cast<const uint8*>(const_cast<FPositionVertexBuffer&>(InPositions).GetVertexData());
		PositionStride = InPositions.GetStride();
		Tangents = reinterpret_cast<const TangentType*>(const_cast<FStaticMeshVertexBuffer&>(InVertices).GetTangentData());
		UVs = reinterpret_cast<const UVType*>(const_cast<FStaticMeshVertexBuffer&>(InVertices).GetTexCoordData());
		NumTexCoords = InVertices.GetNumTexCoords();
		if (InColors.GetNumVertices() > 0)
		{
			checkSlow(InPositions.GetNumVertices() == InColors.GetNumVertices());
			ColorData = (const uint8*)(const_cast<FColorVertexBuffer&>(InColors).GetVertexData());
			ColorStride = InColors.GetStride();
		}
		else
		{
			ColorData = (const uint8*)&FColor::White;
			ColorStride = 0;
		}
	}

	void Blit(FFurStaticVertex<TangentBasisTypeT, UVTypeT>& OutVertex, uint32 InVertexIndex) const
	{
		OutVertex.Position = ((FPositionVertex*)(Positions + InVertexIndex * PositionStride))->Position;
		OutVertex.TangentX = Tangents[InVertexIndex].TangentX;
		OutVertex.TangentZ = Tangents[InVertexIndex].TangentZ;
		OutVertex.UV0 = UVs[NumTexCoords * InVertexIndex].UVs;
		OutVertex.Color = *(FColor*)(ColorData + InVertexIndex * ColorStride);
	}

private:
	const uint8* Positions;
	const TangentType* Tangents;
	const UVType* UVs;
	const uint8* ColorData;
	uint32 PositionStride;
	uint32 NumTexCoords;
	uint32 ColorStride;
};

/** Fur Data Cleanup Task */
template<typename T>
class FFurDataCleanupTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FFurDataCleanupTask<T>>;

public:
	FFurDataCleanupTask(const T& InLambda) : Lambda(InLambda) {}

protected:
	T Lambda;

	void DoWork()
	{
		FPlatformProcess::Sleep(1.0f);
		Lambda();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};

template<typename T>
inline void StartFurDataCleanupTask(const T& InLambda)
{
	(new FAutoDeleteAsyncTask<FFurDataCleanupTask<T>>(InLambda))->StartBackgroundTask();
}
