// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "FurData.h"

/** Fur Static Data */
struct FFurStaticData: public FFurData
{
	UStaticMesh* StaticMesh;
	TArray<UStaticMesh*> GuideMeshes;
	class FFurStaticVertexBuffer* VertexBuffer;

	~FFurStaticData();

	FFurStaticData(UStaticMesh* InStaticMesh, int InLod, UFurSplines* InFurSplines, const TArray<UStaticMesh*>& InGuideMeshes, int InFurLayerCount,
		float InFurLength, float InMinFurLength, float InShellBias, float InHairLengthForceUniformity, float InNoiseStrength, bool InRemoveFacesWithoutSplines);
	static UFurSplines* GenerateSplines(UStaticMesh* InStaticMesh, int InLod, const TArray<UStaticMesh*>& InGuideMeshes);

	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel) override;

	static void ReloadFurSplines(UFurSplines* FurSplines);
	static FFurData* CreateFurData(int InFurLayerCount, int InLod, class UGFurComponent* FurComponent);
	static void DestroyFurData(const TArray<FFurData*>& InFurDataArray);
};
