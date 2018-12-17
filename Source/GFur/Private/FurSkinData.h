// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "Runtime/RenderCore/Public/PackedNormal.h"
#include "FurData.h"

/** Soft Skin Vertex */
struct FSoftSkinVertex
{
	FVector			Position;

	// Tangent, U-direction
	FPackedNormal	TangentX;
	// Binormal, V-direction
	FPackedNormal	TangentY;
	// Normal
	FPackedNormal	TangentZ;

	// UVs
	FVector2D		UVs[MAX_TEXCOORDS];
	// VertexColor
	FColor			Color;
	uint8			InfluenceBones[MAX_TOTAL_INFLUENCES];
	uint8			InfluenceWeights[MAX_TOTAL_INFLUENCES];
};

/** Fur Skin Vertex */
struct FFurSkinVertex : FSoftSkinVertex
{
	FVector FurOffset;
};

/** Fur Skin Vertex Buffer */
class FFurSkinVertexBuffer : public FVertexBuffer
{
public:
	TArray<FFurSkinVertex> Vertices;

	virtual void InitRHI() override;
};

/** Fur Skin Data */
struct FFurSkinData: public FFurData
{
	USkeletalMesh* SkeletalMesh;
	TArray<USkeletalMesh*> GuideMeshes;
	FFurSkinVertexBuffer VertexBuffer;
	bool HasExtraBoneInfluences;

	~FFurSkinData();

	FFurSkinData(USkeletalMesh* InSkeletalMesh, int InLod, UFurSplines* InFurSplines, const TArray<USkeletalMesh*>& InGuideMeshes, int InFurLayerCount,
		float InFurLength, float InMinFurLength, float InShellBias, float InHairLengthForceUniformity, float InNoiseStrength, bool InRemoveFacesWithoutSplines);
	static UFurSplines* GenerateSplines(USkeletalMesh* InSkeletalMesh, int InLod, const TArray<USkeletalMesh*>& InGuideMeshes);

	int NumVertices() const;

	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel) override;

	static void ReloadFurSplines(UFurSplines* FurSplines);
	static FFurSkinData* CreateFurData(int InFurLayerCount, int InLod, class UGFurComponent* FurComponent);
	static void DestroyFurData(const TArray<FFurData*>& InFurDataArray);
};
