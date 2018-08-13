// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "FurData.h"

/** Fur Skin Data */
struct FFurSkinData: public FFurData
{
	USkeletalMesh* SkeletalMesh;
	TArray<USkeletalMesh*> GuideMeshes;
	class FFurSkinVertexBuffer* VertexBuffer;

	~FFurSkinData();

	FFurSkinData(USkeletalMesh* InSkeletalMesh, int InLod, UFurSplines* InFurSplines, const TArray<USkeletalMesh*>& InGuideMeshes, int InFurLayerCount,
		float InFurLength, float InMinFurLength, float InShellBias, float InHairLengthForceUniformity, float InNoiseStrength);
	static UFurSplines* GenerateSplines(USkeletalMesh* InSkeletalMesh, int InLod, const TArray<USkeletalMesh*>& InGuideMeshes);

	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, ERHIFeatureLevel::Type InFeatureLevel) override;

	static void ReloadFurSplines(UFurSplines* FurSplines);
	static FFurData* CreateFurData(int InFurLayerCount, int InLod, class UGFurComponent* FurComponent);
	static void DestroyFurData(const TArray<FFurData*>& InFurDataArray);
};
