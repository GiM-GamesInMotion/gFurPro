// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Components/SkinnedMeshComponent.h"

struct FFurSkinData;

class FFurMorphObject
{
public:
	FFurMorphObject(FFurSkinData* InFurData);
	~FFurMorphObject();

	void Update(FRHICommandListImmediate& RHICmdList, const TArray<FActiveMorphTarget>& ActiveMorphTargets, const TArray<float>& MorphTargetWeights, const TArray<TArray<int32>>& InMorphRemapTable);

	FVertexBuffer* GetVertexBuffer() const { return VertexBuffer; }

private:
	FFurSkinData* FurData;
	FVertexBuffer* VertexBuffer;
};
