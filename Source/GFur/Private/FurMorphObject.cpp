// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurMorphObject.h"
#include "FurSkinData.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Private/SkeletalRenderGPUSkin.h"
#include "Runtime/Engine/Classes/Components/SkinnedMeshComponent.h"
#include "Runtime/Engine/Classes/Animation/MorphTarget.h"
#include "ShaderParameterUtils.h"

void FFurMorphVertexBuffer::InitDynamicRHI()
{
	// Create the buffer rendering resource
	uint32 Size = NumVertices * sizeof(FMorphGPUSkinVertex);
	FRHIResourceCreateInfo CreateInfo;

	EBufferUsageFlags Flags = BUF_Dynamic;

	// BUF_ShaderResource is needed for Morph support of the SkinCache
	Flags = (EBufferUsageFlags)(Flags | BUF_ShaderResource);

	VertexBufferRHI = RHICreateVertexBuffer(Size, Flags, CreateInfo);

	// Lock the buffer.
	void* BufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Size, RLM_WriteOnly);
	FMorphGPUSkinVertex* Buffer = (FMorphGPUSkinVertex*)BufferData;
	FMemory::Memzero(&Buffer[0], Size);
	// Unlock the buffer.
	RHIUnlockVertexBuffer(VertexBufferRHI);
}

void FFurMorphVertexBuffer::ReleaseDynamicRHI()
{
	VertexBufferRHI.SafeRelease();
}

/** Fur Morph Vertex Buffer */
FFurMorphObject::FFurMorphObject(FFurSkinData* InFurData)
{
	FurData = InFurData;
	VertexBuffer.NumVertices = 1;
	BeginInitResource(&VertexBuffer);
}

FFurMorphObject::~FFurMorphObject()
{
	if (VertexBuffer.IsInitialized())
		VertexBuffer.ReleaseResource();
}

void FFurMorphObject::Update_RenderThread(FRHICommandListImmediate& RHICmdList, const TArray<FActiveMorphTarget>& ActiveMorphTargets, const TArray<float>& MorphTargetWeights, const TArray<TArray<int32>>& InMorphRemapTables, int InMeshLod)
{
	int32 NumFurVertices = FurData->GetNumVertices_RenderThread();
	int32 NumVertices = NumFurVertices / FurData->GetFurLayerCount();
	if (NumVertices > 0)
	{
		int32 NumLayers = FurData->GetFurLayerCount();

		uint32 Size = NumVertices * sizeof(FMorphGPUSkinVertex);

		TArray<float> MorphAccumulatedWeightArray;
		FMorphGPUSkinVertex* Buffer = (FMorphGPUSkinVertex*)FMemory::Malloc(Size);

		MorphAccumulatedWeightArray.AddUninitialized(NumVertices);
		FMemory::Memzero(MorphAccumulatedWeightArray.GetData(), sizeof(float) * NumVertices);

		FMemory::Memzero(&Buffer[0], sizeof(FMorphGPUSkinVertex) * NumVertices);

		const auto& MorphRemapTable = InMorphRemapTables[InMeshLod];

		// iterate over all active morph targets and accumulate their vertex deltas
		for (int32 AnimIdx = 0; AnimIdx < ActiveMorphTargets.Num(); AnimIdx++)
		{
			const FActiveMorphTarget& MorphTarget = ActiveMorphTargets[AnimIdx];
			checkSlow(MorphTarget.MorphTarget != NULL);
			checkSlow(MorphTarget.MorphTarget->HasDataForLOD(LODIndex));
			const float MorphTargetWeight = MorphTargetWeights[MorphTarget.WeightIndex];
			const float MorphAbsWeight = FMath::Abs(MorphTargetWeight);
			checkSlow(MorphAbsWeight >= MinMorphTargetBlendWeight && MorphAbsWeight <= MaxMorphTargetBlendWeight);


			// Get deltas
			int32 NumDeltas;
			FMorphTargetDelta* Deltas = MorphTarget.MorphTarget->GetMorphTargetDelta(InMeshLod, NumDeltas);

			// iterate over the vertices that this lod model has changed
			for (int32 MorphVertIdx = 0; MorphVertIdx < NumDeltas; MorphVertIdx++)
			{
				const FMorphTargetDelta& MorphVertex = Deltas[MorphVertIdx];

				// @TODO FIXMELH : temp hack until we fix importing issue
				if (MorphVertex.SourceIdx < (uint32)MorphRemapTable.Num())
				{
					int RemappedIndex = MorphRemapTable[MorphVertex.SourceIdx];
					if (RemappedIndex == -1)
						continue;
					FMorphGPUSkinVertex& DestVertex = Buffer[RemappedIndex];

					DestVertex.DeltaPosition += MorphVertex.PositionDelta * MorphTargetWeight;

					// todo: could be moved out of the inner loop to be more efficient
//						if (bBlendTangentsOnCPU)
					{
						DestVertex.DeltaTangentZ += MorphVertex.TangentZDelta * MorphTargetWeight;
						// accumulate the weight so we can normalized it later
						MorphAccumulatedWeightArray[RemappedIndex] += MorphAbsWeight;
					}
				}
			} // for all vertices
		} // for all morph targets

//		if (bBlendTangentsOnCPU)
		{
			// copy back all the tangent values (can't use Memcpy, since we have to pack the normals)
			for (int32 iVertex = 0; iVertex < NumVertices; ++iVertex)
			{
				FMorphGPUSkinVertex& DestVertex = Buffer[iVertex];
				float AccumulatedWeight = MorphAccumulatedWeightArray[iVertex];

				// if accumulated weight is >1.f
				// previous code was applying the weight again in GPU if less than 1, but it doesn't make sense to do so
				// so instead, we just divide by AccumulatedWeight if it's more than 1.
				// now DeltaTangentZ isn't FPackedNormal, so you can apply any value to it. 
				if (AccumulatedWeight > 1.f)
				{
					DestVertex.DeltaTangentZ /= AccumulatedWeight;
				}
			}
		}

		// Lock the real buffer.
		{
			if (!VertexBuffer.IsInitialized())
			{
				VertexBuffer.NumVertices = NumFurVertices;
				VertexBuffer.InitResource();
			}
			else if (VertexBuffer.NumVertices != NumFurVertices)
			{
				VertexBuffer.NumVertices = NumFurVertices;
				VertexBuffer.ReleaseDynamicRHI();
				VertexBuffer.InitDynamicRHI();
			}

			FMorphGPUSkinVertex* ActualBuffer = (FMorphGPUSkinVertex*)RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, Size * NumLayers, RLM_WriteOnly);
			for (const auto& Section : FurData->GetSections_RenderThread())
			{
				uint32 NumSectionVertices = Section.MaxVertexIndex - Section.MinVertexIndex + 1;
				check(NumSectionVertices % NumLayers == 0);
				FMorphGPUSkinVertex* SectionBuffer = ActualBuffer + Section.MinVertexIndex;
				uint32 NumLayerVertices = NumSectionVertices / NumLayers;

				const auto* SourceBuffer = Buffer + (Section.MinVertexIndex / NumLayers);

				uint32 LayerSize = NumLayerVertices * sizeof(FMorphGPUSkinVertex);

				for (int i = 0; i < NumLayers; i++)
					FMemory::Memcpy(SectionBuffer + NumLayerVertices * i, SourceBuffer, LayerSize);
			}
			FMemory::Free(Buffer);
		}

		{
			// Unlock the buffer.
			RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);
			// set update flag
//			MorphVertexBuffer.bHasBeenUpdated = true;
		}
	}
}
