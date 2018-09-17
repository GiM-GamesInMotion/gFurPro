// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Components/SkinnedMeshComponent.h"

class FFurMorphObject
{
public:
	FFurMorphObject(int InNumVertices, int InNumLayers, int InLodIndex);
	~FFurMorphObject();

	void Update(FRHICommandListImmediate& RHICmdList, const TArray<FActiveMorphTarget>& ActiveMorphTargets, const TArray<float>& MorphTargetWeights, const TArray<TArray<int32>>& InMorphRemapTable);

	FVertexBuffer* GetVertexBuffer() const { return VertexBuffer; }

private:
	FVertexBuffer* VertexBuffer;
	int NumVertices;
	int NumLayers;
	int LODIndex;
};
