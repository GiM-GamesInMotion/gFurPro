// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurSkinData.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Private/SkeletalRenderGPUSkin.h"
#include "ShaderParameterUtils.h"
#include "FurComponent.h"

static TArray< FFurSkinData* > FurSkinData;
static FCriticalSection FurSkinDataCS;

unsigned int MaxGPUSkinBones = 256;

/** Fur Skin Vertex Blitter */
template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT, bool bExtraBoneInfluencesT>
class FFurSkinVertexBlitter : public FFurStaticVertexBlitter<TangentBasisTypeT, UVTypeT>
{
public:
	FFurSkinVertexBlitter(const FPositionVertexBuffer& InPositions, const FStaticMeshVertexBuffer& InVertices, const FColorVertexBuffer& InColors, const FSkinWeightVertexBuffer& InSkinWeights)
		: FFurStaticVertexBlitter<TangentBasisTypeT, UVTypeT>(InPositions, InVertices, InColors), SkinWeights(InSkinWeights)
	{}

	void Blit(FFurSkinVertex<TangentBasisTypeT, UVTypeT, bExtraBoneInfluencesT>& OutVertex, uint32 InVertexIndex) const
	{
		FFurStaticVertexBlitter<TangentBasisTypeT, UVTypeT>::Blit(OutVertex, InVertexIndex);

		const auto* WeightInfo = SkinWeights.GetSkinWeightPtr<bExtraBoneInfluencesT>(InVertexIndex);
		const int32 NumInfluences = FFurSkinVertex<TangentBasisTypeT, UVTypeT, bExtraBoneInfluencesT>::NumInfluences;
		for (int32 ib = 0; ib < NumInfluences; ib++)
		{
			OutVertex.InfluenceBones[ib] = WeightInfo->InfluenceBones[ib];
			OutVertex.InfluenceWeights[ib] = WeightInfo->InfluenceWeights[ib];
		}
	}

private:
	const FSkinWeightVertexBuffer& SkinWeights;
};

/** Vertex Factory Shader Parameters */
template <bool Physics>
class FFurSkinVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
public:
	void Bind(const FShaderParameterMap& ParameterMap) override
	{
		MeshOriginParameter.Bind(ParameterMap, TEXT("MeshOrigin"));
		MeshExtensionParameter.Bind(ParameterMap, TEXT("MeshExtension"));
		FurOffsetPowerParameter.Bind(ParameterMap, TEXT("FurOffsetPower"));
		MaxPhysicsOffsetLengthParameter.Bind(ParameterMap, TEXT("MaxPhysicsOffsetLength"));
		BoneMatrices.Bind(ParameterMap, TEXT("BoneMatrices"));
		PreviousBoneMatrices.Bind(ParameterMap, TEXT("PreviousBoneMatrices"));
		BoneFurOffsets.Bind(ParameterMap, TEXT("BoneFurOffsets"));
		PreviousBoneFurOffsets.Bind(ParameterMap, TEXT("PreviousBoneFurOffsets"));
	}


	void Serialize(FArchive& Ar) override
	{
		Ar << MeshOriginParameter;
		Ar << MeshExtensionParameter;
		Ar << FurOffsetPowerParameter;
		Ar << MaxPhysicsOffsetLengthParameter;
		Ar << BoneMatrices;
		Ar << PreviousBoneMatrices;
		Ar << BoneFurOffsets;
		Ar << PreviousBoneFurOffsets;
	}


	/**
	* Set any shader data specific to this vertex factory
	*/
	virtual void SetMesh(FRHICommandList& RHICmdList, FShader* Shader, const FVertexFactory* VertexFactory, const FSceneView& View, const FMeshBatchElement& BatchElement, uint32 DataFlags) const override;

	uint32 GetSize() const override { return sizeof(*this); }

private:
	FShaderParameter MeshOriginParameter;
	FShaderParameter MeshExtensionParameter;
	FShaderParameter FurOffsetPowerParameter;
	FShaderParameter MaxPhysicsOffsetLengthParameter;
	FShaderResourceParameter BoneMatrices;
	FShaderResourceParameter PreviousBoneMatrices;
	FShaderResourceParameter BoneFurOffsets;
	FShaderResourceParameter PreviousBoneFurOffsets;
};

/** Vertex Factory */
template<bool MorphTargets, bool Physics, bool bExtraInfluencesT>
class FFurSkinVertexFactoryBase : public FFurVertexFactory
{
	typedef FFurSkinVertexFactoryBase<MorphTargets, Physics, bExtraInfluencesT> This;

public:
	struct FShaderDataType
	{
		FShaderDataType(ERHIFeatureLevel::Type InFeatureLevel)
			: CurrentBuffer(0)
			, MeshOrigin(0, 0, 0)
			, MeshExtension(1, 1, 1)
			, FurOffsetPower(2.0f)
			, MaxPhysicsOffsetLength(FLT_MAX)
			, FeatureLevel(InFeatureLevel)
			, Discontinuous(true)
		{
		}

		/** Mesh origin and Mesh Extension for Mesh compressions **/
		/** This value will be (0, 0, 0), (1, 1, 1) relatively for non compressed meshes **/
		FVector MeshOrigin;
		FVector MeshExtension;
		float FurOffsetPower;
		float MaxPhysicsOffsetLength;

		void Init(uint32 InBoneCount)
		{
			BoneCount = InBoneCount;
		}

		// @param FrameTime from GFrameTime
		void UpdateBoneData(const TArray<FMatrix>& ReferenceToLocalMatrices, const TArray<FVector>& LinearOffsets, const TArray<FVector>& AngularOffsets,
			const TArray<FMatrix>& LastTransformations, const TArray<FBoneIndexType>& BoneMap, bool InDiscontinuous, ERHIFeatureLevel::Type FeatureLevel);

		void InitDynamicRHI();

		void ReleaseBoneData()
		{
			ensure(IsInRenderingThread());

			UniformBuffer.SafeRelease();

			for (uint32 i = 0; i < 2; ++i)
			{
				BoneBuffer[i].SafeRelease();
				BoneFurOffsetsBuffer[i].SafeRelease();
			}
		}

		// if FeatureLevel < ERHIFeatureLevel::ES3_1
		FUniformBufferRHIParamRef GetUniformBuffer() const
		{
			return UniformBuffer;
		}

		// @param bPrevious true:previous, false:current
		// @param FrameNumber usually from View.Family->FrameNumber
		const FVertexBufferAndSRV& GetBoneBufferForReading(bool bPrevious) const
		{
			const FVertexBufferAndSRV* RetPtr = &GetBoneBufferInternal(bPrevious);

			if (!RetPtr->VertexBufferRHI.IsValid())
			{
				// this only should happen if we request the old data
				check(bPrevious);

				// if we don't have any old data we use the current one
				RetPtr = &GetBoneBufferInternal(false);

				// at least the current one needs to be valid when reading
				check(RetPtr->VertexBufferRHI.IsValid());
			}

			return *RetPtr;
		}

		// @param bPrevious true:previous, false:current
		// @param FrameNumber usually from View.Family->FrameNumber
		// @return IsValid() can fail, then you have to create the buffers first (or if the size changes)
		FVertexBufferAndSRV& GetBoneBufferForWriting(bool bPrevious)
		{
			const FShaderDataType* This = (const FShaderDataType*)this;

			// non const version maps to const version
			return (FVertexBufferAndSRV&)This->GetBoneBufferInternal(bPrevious);
		}

		const FVertexBufferAndSRV& GetBoneFurOffsetsBufferForReading(bool bPrevious) const
		{
			const FVertexBufferAndSRV* RetPtr = &GetBoneFurOffsetsBufferInternal(bPrevious);

			if (!RetPtr->VertexBufferRHI.IsValid())
			{
				// this only should happen if we request the old data
				check(bPrevious);

				// if we don't have any old data we use the current one
				RetPtr = &GetBoneFurOffsetsBufferInternal(false);

				// at least the current one needs to be valid when reading
				check(RetPtr->VertexBufferRHI.IsValid());
			}

			return *RetPtr;
		}

		FVertexBufferAndSRV& GetBoneFurOffsetsBufferForWriting(bool bPrevious)
		{
			const FShaderDataType* This = (const FShaderDataType*)this;

			// non const version maps to const version
			return (FVertexBufferAndSRV&)This->GetBoneFurOffsetsBufferInternal(bPrevious);
		}

	private:
		// double buffered bone positions+orientations to support normal rendering and velocity (new-old position) rendering
		FVertexBufferAndSRV BoneBuffer[2];
		FVertexBufferAndSRV BoneFurOffsetsBuffer[2];
		// 0 / 1 to index into BoneBuffer
		uint32 CurrentBuffer;
		// if FeatureLevel < ERHIFeatureLevel::ES3_1
		FUniformBufferRHIRef UniformBuffer;
		uint32 BoneCount;
		ERHIFeatureLevel::Type FeatureLevel;
		bool Discontinuous;

		void GoToNextFrame(bool InDiscontinuous);

		// to support GetBoneBufferForWriting() and GetBoneBufferForReading()
		// @param bPrevious true:previous, false:current
		// @param FrameNumber usually from View.Family->FrameNumber
		// @return might not pass the IsValid() 
		const FVertexBufferAndSRV& GetBoneBufferInternal(bool bPrevious) const
		{
			check(IsInParallelRenderingThread());

			if (Discontinuous)
			{
				bPrevious = false;
			}

			uint32 BufferIndex = CurrentBuffer ^ (uint32)bPrevious;

			const FVertexBufferAndSRV& Ret = BoneBuffer[BufferIndex];
			return Ret;
		}

		const FVertexBufferAndSRV& GetBoneFurOffsetsBufferInternal(bool bPrevious) const
		{
			check(IsInParallelRenderingThread());

			if (Discontinuous)
			{
				bPrevious = false;
			}

			uint32 BufferIndex = CurrentBuffer ^ (uint32)bPrevious;

			const FVertexBufferAndSRV& Ret = BoneFurOffsetsBuffer[BufferIndex];
			return Ret;
		}
	};

	FFurSkinVertexFactoryBase(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurVertexFactory(InFeatureLevel)
		, ShaderData(InFeatureLevel)
	{
	}

	struct FDataType
	{
		FVertexStreamComponent PositionComponent;
		FVertexStreamComponent TangentBasisComponents[2];
		TArray<FVertexStreamComponent, TFixedAllocator<MAX_TEXCOORDS>> TextureCoordinates;
		FVertexStreamComponent ColorComponent;
		FVertexStreamComponent BoneIndices;
		FVertexStreamComponent BoneIndicesExtra;
		FVertexStreamComponent BoneWeights;
		FVertexStreamComponent BoneWeightsExtra;
		FVertexStreamComponent FurOffset;

		FVertexStreamComponent DeltaPosition;
		FVertexStreamComponent DeltaTangentZ;
	};

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
	void Init(const FFurVertexBuffer* VertexBuffer, const FVertexBuffer* MorphVertexBuffer, uint32 BoneCount)
	{
		typedef FFurSkinVertex<TangentBasisTypeT, UVTypeT, bExtraInfluencesT> VertexType;
		ShaderData.Init(BoneCount);
		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(InitProceduralMeshVertexFactory,
			FFurSkinVertexFactoryBase*, VertexFactory, this,
			const FFurVertexBuffer*, VertexBuffer, VertexBuffer,
			const FVertexBuffer*, MorphVertexBuffer, MorphVertexBuffer,
			{
				const auto TangentElementType = TStaticMeshVertexTangentTypeSelector<TangentBasisTypeT>::VertexElementType;
				const auto UvElementType = UVTypeT == EStaticMeshVertexUVType::HighPrecision ? VET_Float2 : VET_Half2;

				// Initialize the vertex factory's stream components.
				FDataType NewData;
				NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, Position, VET_Float3);
				NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, UV0), sizeof(VertexType), UvElementType));
				NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, UV1), sizeof(VertexType), VET_Float2));
				NewData.TextureCoordinates.Add(FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, UV2), sizeof(VertexType), VET_Float2));
				NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, TangentX, TangentElementType);
				NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, TangentZ, TangentElementType);
				NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, Color, VET_Color);
				NewData.BoneIndices = FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, InfluenceBones), sizeof(VertexType), VET_UByte4);
				if (bExtraInfluencesT)
					NewData.BoneIndicesExtra = FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, InfluenceBones) + 4, sizeof(VertexType), VET_UByte4);
				NewData.BoneWeights = FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, InfluenceWeights), sizeof(VertexType), VET_UByte4N);
				if (bExtraInfluencesT)
					NewData.BoneWeightsExtra = FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(VertexType, InfluenceWeights) + 4, sizeof(VertexType), VET_UByte4N);
				NewData.FurOffset = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, FurOffset, VET_Float3);

				if (MorphTargets)
				{
					NewData.DeltaPosition = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(MorphVertexBuffer, FMorphGPUSkinVertex, DeltaPosition, VET_Float3);
					NewData.DeltaTangentZ = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(MorphVertexBuffer, FMorphGPUSkinVertex, DeltaTangentZ, VET_Float3);
				}

				VertexFactory->SetData(NewData);
			});
	}

	static FVertexFactoryShaderParameters* ConstructShaderParameters(EShaderFrequency ShaderFrequency)
	{
		return (ShaderFrequency == SF_Vertex) ? new FFurSkinVertexFactoryShaderParameters<Physics>() : NULL;
	}


	static bool ShouldCache(EShaderPlatform Platform,
		const class FMaterial* Material,
		const class FShaderType* ShaderType)
	{
		return (Material->IsUsedWithSkeletalMesh() || Material->IsSpecialEngineMaterial());
	}

	static void ModifyCompilationEnvironment(EShaderPlatform Platform, const FMaterial* Material, FShaderCompilerEnvironment& OutEnvironment)
	{
//		Super::ModifyCompilationEnvironment(Platform, Material, OutEnvironment);
		if (MorphTargets)
			OutEnvironment.SetDefine(TEXT("GPUSKIN_MORPH_BLEND"), TEXT("1"));
		if (Physics)
			OutEnvironment.SetDefine(TEXT("GFUR_PHYSICS"), TEXT("1"));
		if (bExtraInfluencesT)
			OutEnvironment.SetDefine(TEXT("GPUSKIN_USE_EXTRA_INFLUENCES"), TEXT("1"));
	}

	static bool ShouldCompilePermutation(EShaderPlatform Platform, const class FMaterial* Material, const FShaderType* ShaderType)
	{
		return (Material->IsUsedWithSkeletalMesh() || Material->IsSpecialEngineMaterial()) || (MorphTargets ? Material->IsUsedWithMorphTargets() : false);
	}

	void SetData(const FDataType& InData)
	{
		Data = InData;
		FVertexFactory::UpdateRHI();
	}

	void AddVertexElements(FDataType& InData, FVertexDeclarationElementList& OutElements)
	{
		OutElements.Add(AccessStreamComponent(InData.PositionComponent, 0));

		OutElements.Add(AccessStreamComponent(InData.TangentBasisComponents[0], 1));
		OutElements.Add(AccessStreamComponent(InData.TangentBasisComponents[1], 2));

		if (InData.TextureCoordinates.Num())
		{
			const uint8 BaseTexCoordAttribute = 5;
			for (int32 CoordinateIndex = 0; CoordinateIndex < InData.TextureCoordinates.Num(); CoordinateIndex++)
			{
				OutElements.Add(AccessStreamComponent(
					InData.TextureCoordinates[CoordinateIndex], BaseTexCoordAttribute + CoordinateIndex));
			}

			for (int32 CoordinateIndex = InData.TextureCoordinates.Num(); CoordinateIndex < MAX_TEXCOORDS;
				CoordinateIndex++)
			{
				OutElements.Add(AccessStreamComponent(InData.TextureCoordinates[InData.TextureCoordinates.Num() - 1],
					BaseTexCoordAttribute + CoordinateIndex));
			}
		}

		if (InData.ColorComponent.VertexBuffer)
		{
			OutElements.Add(AccessStreamComponent(InData.ColorComponent, 13));
		}
		else
		{
			FVertexStreamComponent NullColorComponent(&GNullColorVertexBuffer, 0, 0, VET_Color);
			OutElements.Add(AccessStreamComponent(NullColorComponent, 13));
		}
		OutElements.Add(AccessStreamComponent(InData.BoneIndices, 3));
		OutElements.Add(AccessStreamComponent(InData.BoneWeights, 4));
		OutElements.Add(AccessStreamComponent(InData.FurOffset, 12));

		if (MorphTargets)
		{
			OutElements.Add(AccessStreamComponent(InData.DeltaPosition, 9));
			OutElements.Add(AccessStreamComponent(InData.DeltaTangentZ, 10));
		}

		if (bExtraInfluencesT)
		{
			OutElements.Add(AccessStreamComponent(InData.BoneIndicesExtra, 14));
			OutElements.Add(AccessStreamComponent(InData.BoneWeightsExtra, 15));
		}
	}

	void InitRHI() override
	{
		// list of declaration items
		FVertexDeclarationElementList Elements;
		AddVertexElements(Data, Elements);

		// create the actual device decls
		InitDeclaration(Elements);
	}

	void InitDynamicRHI() override
	{
		FVertexFactory::InitDynamicRHI();
		ShaderData.InitDynamicRHI();
	}

	void ReleaseDynamicRHI() override
	{
		FVertexFactory::ReleaseDynamicRHI();
		ShaderData.ReleaseBoneData();
	}

	void UpdateSkeletonShaderData(float InFurOffsetPower, float InMaxPhysicsOffsetLength, const TArray<FMatrix>& InReferenceToLocal, const TArray<FVector>& InLinearOffsets, const TArray<FVector>& InAngularOffsets,
		const TArray<FMatrix>& InTransformations, const TArray<FBoneIndexType>& InBoneMap, bool InDiscontinuous, ERHIFeatureLevel::Type InFeatureLevel) override
	{
		ShaderData.FurOffsetPower = InFurOffsetPower;
		ShaderData.MaxPhysicsOffsetLength = InMaxPhysicsOffsetLength;
		ShaderData.UpdateBoneData(InReferenceToLocal, InLinearOffsets, InAngularOffsets, InTransformations, InBoneMap, InDiscontinuous, InFeatureLevel);
	}

	FDataType Data;
	FShaderDataType ShaderData;
};

class FMorphPhysicsExtraInfluencesFurSkinVertexFactory : public FFurSkinVertexFactoryBase<true, true, true>
{
	DECLARE_VERTEX_FACTORY_TYPE(FMorphPhysicsExtraInfluencesFurSkinVertexFactory);
public:
	FMorphPhysicsExtraInfluencesFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<true, true, true>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<true, true, true>::Init;
};

class FPhysicsExtraInfluencesFurSkinVertexFactory : public FFurSkinVertexFactoryBase<false, true, true>
{
	DECLARE_VERTEX_FACTORY_TYPE(FPhysicsExtraInfluencesFurSkinVertexFactory);
public:
	FPhysicsExtraInfluencesFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<false, true, true>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<false, true, true>::Init;
};

class FMorphExtraInfluencesFurSkinVertexFactory : public FFurSkinVertexFactoryBase<true, false, true>
{
	DECLARE_VERTEX_FACTORY_TYPE(FMorphExtraInfluencesFurSkinVertexFactory);
public:
	FMorphExtraInfluencesFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<true, false, true>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<true, false, true>::Init;
};

class FExtraInfluencesFurSkinVertexFactory : public FFurSkinVertexFactoryBase<false, false, true>
{
	DECLARE_VERTEX_FACTORY_TYPE(FExtraInfluencesFurSkinVertexFactory);
public:
	FExtraInfluencesFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<false, false, true>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<false, false, true>::Init;
};

class FMorphPhysicsFurSkinVertexFactory : public FFurSkinVertexFactoryBase<true, true, false>
{
	DECLARE_VERTEX_FACTORY_TYPE(FMorphPhysicsFurSkinVertexFactory);
public:
	FMorphPhysicsFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<true, true, false>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<true, true, false>::Init;
};

class FPhysicsFurSkinVertexFactory : public FFurSkinVertexFactoryBase<false, true, false>
{
	DECLARE_VERTEX_FACTORY_TYPE(FPhysicsFurSkinVertexFactory);
public:
	FPhysicsFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<false, true, false>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<false, true, false>::Init;
};

class FMorphFurSkinVertexFactory : public FFurSkinVertexFactoryBase<true, false, false>
{
	DECLARE_VERTEX_FACTORY_TYPE(FMorphFurSkinVertexFactory);
public:
	FMorphFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<true, false, false>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<true, false, false>::Init;
};

class FFurSkinVertexFactory : public FFurSkinVertexFactoryBase<false, false, false>
{
	DECLARE_VERTEX_FACTORY_TYPE(FFurSkinVertexFactory);
public:
	FFurSkinVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurSkinVertexFactoryBase<false, false, false>(InFeatureLevel)
	{
	}

	using FFurSkinVertexFactoryBase<false, false, false>::Init;
};

IMPLEMENT_VERTEX_FACTORY_TYPE(FMorphPhysicsExtraInfluencesFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FPhysicsExtraInfluencesFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FMorphExtraInfluencesFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FExtraInfluencesFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FMorphPhysicsFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FPhysicsFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FMorphFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FFurSkinVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);

#if WITH_EDITORONLY_DATA
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FBoneMatricesUniformShaderParameters, TEXT("Bones"));
#endif // WITH_EDITORONLY_DATA

static FBoneMatricesUniformShaderParameters GBoneUniformStruct;

template<bool MorphTargets, bool Physics, bool ExtraInfluences>
void FFurSkinVertexFactoryBase<MorphTargets, Physics, ExtraInfluences>::FShaderDataType::GoToNextFrame(bool InDiscontinuous)
{
	CurrentBuffer = 1 - CurrentBuffer;
	Discontinuous = InDiscontinuous;
}

template<bool MorphTargets, bool Physics, bool ExtraInfluences>
void FFurSkinVertexFactoryBase<MorphTargets, Physics, ExtraInfluences>::FShaderDataType::UpdateBoneData(const TArray<FMatrix>& ReferenceToLocalMatrices, const TArray<FVector>& LinearOffsets, const TArray<FVector>& AngularOffsets,
	const TArray<FMatrix>& LastTransformations, const TArray<FBoneIndexType>& BoneMap, bool InDiscontinuous, ERHIFeatureLevel::Type InFeatureLevel)
{
	const uint32 NumBones = BoneMap.Num();
	check(NumBones <= MaxGPUSkinBones);
	FSkinMatrix3x4* ChunkMatrices = nullptr;
	FVector4* Offsets = nullptr;

	FVertexBufferAndSRV* CurrentBoneBuffer = 0;
	FVertexBufferAndSRV* CurrentBoneFurOffsetsBuffer = 0;

	if (InFeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		check(IsInRenderingThread());
		GoToNextFrame(InDiscontinuous);

		CurrentBoneBuffer = &GetBoneBufferForWriting(false);

		uint32 NumVectors = NumBones * 3;
		check(NumVectors <= (MaxGPUSkinBones * 3));
		uint32 VectorArraySize = NumVectors * sizeof(FVector4);

		uint32 OffsetArraySize = NumBones * 3 * sizeof(FVector4);

		if (!IsValidRef(*CurrentBoneBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(VectorArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*CurrentBoneBuffer = Buffer;
			check(IsValidRef(*CurrentBoneBuffer));
		}

		CurrentBoneFurOffsetsBuffer = &GetBoneFurOffsetsBufferForWriting(false);
		if (!IsValidRef(*CurrentBoneFurOffsetsBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(OffsetArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*CurrentBoneFurOffsetsBuffer = Buffer;
			check(IsValidRef(*CurrentBoneFurOffsetsBuffer));
		}

		if (NumBones)
		{
			ChunkMatrices = (FSkinMatrix3x4*)RHILockVertexBuffer(CurrentBoneBuffer->VertexBufferRHI, 0, VectorArraySize, RLM_WriteOnly);
			Offsets = (FVector4*)RHILockVertexBuffer(CurrentBoneFurOffsetsBuffer->VertexBufferRHI, 0, OffsetArraySize, RLM_WriteOnly);
		}
	}
	else
	{
		if (NumBones)
		{
			check(NumBones * sizeof(FSkinMatrix3x4) <= sizeof(GBoneUniformStruct));
			ChunkMatrices = (FSkinMatrix3x4*)&GBoneUniformStruct;
		}
	}

	if (InFeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		//FSkinMatrix3x4 is sizeof() == 48
		// PLATFORM_CACHE_LINE_SIZE (128) / 48 = 2.6
		//  sizeof(FMatrix) == 64
		// PLATFORM_CACHE_LINE_SIZE (128) / 64 = 2
		const int32 PreFetchStride = 2; // FPlatformMisc::Prefetch stride
		for (uint32 BoneIdx = 0; BoneIdx < NumBones; BoneIdx++)
		{
			const FBoneIndexType RefToLocalIdx = BoneMap[BoneIdx];
			FPlatformMisc::Prefetch(ReferenceToLocalMatrices.GetData() + RefToLocalIdx + PreFetchStride);
			FPlatformMisc::Prefetch(ReferenceToLocalMatrices.GetData() + RefToLocalIdx + PreFetchStride, PLATFORM_CACHE_LINE_SIZE);

			FSkinMatrix3x4& BoneMat = ChunkMatrices[BoneIdx];
			const FMatrix& RefToLocal = ReferenceToLocalMatrices[RefToLocalIdx];
			RefToLocal.To3x4MatrixTranspose((float*)BoneMat.M);

			Offsets[BoneIdx * 3] = LinearOffsets[RefToLocalIdx];
			Offsets[BoneIdx * 3 + 1] = AngularOffsets[RefToLocalIdx];
			Offsets[BoneIdx * 3 + 2] = LastTransformations[RefToLocalIdx].GetOrigin();
		}
	}
	else
	{
		const int32 PreFetchStride = 2; // FPlatformMisc::Prefetch stride
		for (uint32 BoneIdx = 0; BoneIdx < NumBones; BoneIdx++)
		{
			const FBoneIndexType RefToLocalIdx = BoneMap[BoneIdx];
			FPlatformMisc::Prefetch(ReferenceToLocalMatrices.GetData() + RefToLocalIdx + PreFetchStride);
			FPlatformMisc::Prefetch(ReferenceToLocalMatrices.GetData() + RefToLocalIdx + PreFetchStride, PLATFORM_CACHE_LINE_SIZE);

			FSkinMatrix3x4& BoneMat = ChunkMatrices[BoneIdx];
			const FMatrix& RefToLocal = ReferenceToLocalMatrices[RefToLocalIdx];
			RefToLocal.To3x4MatrixTranspose((float*)BoneMat.M);
		}
	}
	if (InFeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		if (NumBones)
		{
			check(CurrentBoneBuffer);
			RHIUnlockVertexBuffer(CurrentBoneBuffer->VertexBufferRHI);
			check(CurrentBoneFurOffsetsBuffer);
			RHIUnlockVertexBuffer(CurrentBoneFurOffsetsBuffer->VertexBufferRHI);
		}
	}
	else
	{
		UniformBuffer = RHICreateUniformBuffer(&GBoneUniformStruct, FBoneMatricesUniformShaderParameters::StaticStruct.GetLayout(), UniformBuffer_MultiFrame);
	}
}

template<bool MorphTargets, bool Physics, bool ExtraInfluences>
void FFurSkinVertexFactoryBase<MorphTargets, Physics, ExtraInfluences>::FShaderDataType::InitDynamicRHI()
{
	const uint32 NumBones = BoneCount;
	check(NumBones <= MaxGPUSkinBones);
	FSkinMatrix3x4* ChunkMatrices = nullptr;

	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		check(IsInRenderingThread());

		uint32 NumVectors = NumBones * 3;
		check(NumVectors <= (MaxGPUSkinBones * 3));
		uint32 VectorArraySize = NumVectors * sizeof(FVector4);

		uint32 OffsetArraySize = NumBones * 3 * sizeof(FVector4);

		FVertexBufferAndSRV* CurrentBoneBuffer = &GetBoneBufferForWriting(false);
		if (!IsValidRef(*CurrentBoneBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(VectorArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*CurrentBoneBuffer = Buffer;
			check(IsValidRef(*CurrentBoneBuffer));
		}

		FVertexBufferAndSRV* PreviousBoneBuffer = &GetBoneBufferForWriting(true);
		if (!IsValidRef(*PreviousBoneBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(VectorArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*PreviousBoneBuffer = Buffer;
			check(IsValidRef(*PreviousBoneBuffer));
		}

		FVertexBufferAndSRV* CurrentBoneFurOffsetsBuffer = &GetBoneFurOffsetsBufferForWriting(false);
		if (!IsValidRef(*CurrentBoneFurOffsetsBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(OffsetArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*CurrentBoneFurOffsetsBuffer = Buffer;
			check(IsValidRef(*CurrentBoneFurOffsetsBuffer));
		}

		FVertexBufferAndSRV* PreviousBoneFurOffsetsBuffer = &GetBoneFurOffsetsBufferForWriting(true);
		if (!IsValidRef(*PreviousBoneFurOffsetsBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(OffsetArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*PreviousBoneFurOffsetsBuffer = Buffer;
			check(IsValidRef(*PreviousBoneFurOffsetsBuffer));
		}
	}
	else
	{
		if (NumBones)
		{
			check(NumBones * sizeof(FSkinMatrix3x4) <= sizeof(GBoneUniformStruct));
			ChunkMatrices = (FSkinMatrix3x4*)&GBoneUniformStruct;
		}
	}

	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
	}
	else
	{
		UniformBuffer = RHICreateUniformBuffer(&GBoneUniformStruct, FBoneMatricesUniformShaderParameters::StaticStruct.GetLayout(), UniformBuffer_MultiFrame);
	}
}

template<bool Physics>
void FFurSkinVertexFactoryShaderParameters<Physics>::SetMesh(FRHICommandList& RHICmdList, FShader* Shader, const FVertexFactory* VertexFactory, const FSceneView& View, const FMeshBatchElement& BatchElement, uint32 DataFlags) const
{
	FRHIVertexShader* ShaderRHI = Shader->GetVertexShader();

	if (ShaderRHI)
	{
		FFurSkinVertexFactory::FShaderDataType& ShaderData = ((FFurSkinVertexFactory*)VertexFactory)->ShaderData;

		SetShaderValue(RHICmdList, ShaderRHI, MeshOriginParameter, ShaderData.MeshOrigin);
		SetShaderValue(RHICmdList, ShaderRHI, MeshExtensionParameter, ShaderData.MeshExtension);
		SetShaderValue(RHICmdList, ShaderRHI, FurOffsetPowerParameter, ShaderData.FurOffsetPower);
		SetShaderValue(RHICmdList, ShaderRHI, MaxPhysicsOffsetLengthParameter, ShaderData.MaxPhysicsOffsetLength);

//		const auto FeatureLevel = View.GetFeatureLevel();

		if (BoneMatrices.IsBound())
		{
			FShaderResourceViewRHIParamRef CurrentData = ShaderData.GetBoneBufferForReading(false).VertexBufferSRV;
			RHICmdList.SetShaderResourceViewParameter(ShaderRHI, BoneMatrices.GetBaseIndex(), CurrentData);
		}
		if (PreviousBoneMatrices.IsBound())
		{
			// todo: Maybe a check for PreviousData!=CurrentData would save some performance (when objects don't have velocty yet) but removing the bool also might save performance

			FShaderResourceViewRHIParamRef PreviousData = ShaderData.GetBoneBufferForReading(true).VertexBufferSRV;
			RHICmdList.SetShaderResourceViewParameter(ShaderRHI, PreviousBoneMatrices.GetBaseIndex(), PreviousData);
		}

		if (Physics)
		{
			if (BoneFurOffsets.IsBound())
			{
				FShaderResourceViewRHIParamRef CurrentData = ShaderData.GetBoneFurOffsetsBufferForReading(false).VertexBufferSRV;
				RHICmdList.SetShaderResourceViewParameter(ShaderRHI, BoneFurOffsets.GetBaseIndex(), CurrentData);
			}
			if (PreviousBoneFurOffsets.IsBound())
			{
				FShaderResourceViewRHIParamRef PreviousData = ShaderData.GetBoneFurOffsetsBufferForReading(true).VertexBufferSRV;
				RHICmdList.SetShaderResourceViewParameter(ShaderRHI, PreviousBoneFurOffsets.GetBaseIndex(), PreviousData);
			}
		}
		else
		{
			SetUniformBufferParameter(RHICmdList, ShaderRHI, Shader->GetUniformBufferParameter<FBoneMatricesUniformShaderParameters>(), ShaderData.GetUniformBuffer());
		}
	}
}

/** Fur Skin Data */
FFurSkinData* FFurSkinData::CreateFurData(int32 InFurLayerCount, int32 InLod, UGFurComponent* InFurComponent)
{
	check(InFurLayerCount >= MinimalFurLayerCount && InFurLayerCount <= MaximalFurLayerCount);

	FScopeLock lock(&FurSkinDataCS);

	for (FFurSkinData* Data : FurSkinData)
	{
		if (Data->Compare(InFurLayerCount, InLod, InFurComponent))
		{
			Data->RefCount++;
			return Data;
		}
	}
	for (FFurSkinData* Data : FurSkinData)
	{
		if (Data->RefCount == 0 && Data->Similar(InLod, InFurComponent))
		{
			Data->Set(InFurLayerCount, InLod, InFurComponent);
			Data->BuildFur(BuildType::Minimal);
			Data->RefCount++;
			return Data;
		}
	}

	FFurSkinData* Data = new FFurSkinData();
	Data->Set(InFurLayerCount, InLod, InFurComponent);
	Data->BuildFur(BuildType::Full);
	FurSkinData.Add(Data);
	return Data;
}

void FFurSkinData::DestroyFurData(const TArray<FFurData*>& InFurDataArray)
{
	FScopeLock lock(&FurSkinDataCS);

	for (auto* Data : InFurDataArray)
		((FFurSkinData*)Data)->RefCount--;

	StartFurDataCleanupTask([]() {

		FScopeLock lock(&FurSkinDataCS);

		for (int32 i = FurSkinData.Num() - 1; i >= 0; i--)
		{
			FFurSkinData* Data = FurSkinData[i];
			if (Data->RefCount == 0)
			{
				FurSkinData.RemoveAt(i);
				ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([Data](FRHICommandListImmediate& RHICmdList) { delete Data; });
			}
		}
	});
}

void FFurSkinData::CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel)
{
	auto CreateVertexFactory = [&](const FFurData::FSection& s, auto* vf) {
		if (bUseHighPrecisionTangentBasis)
		{
			if (bUseFullPrecisionUVs)
				vf->Init<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::HighPrecision>(&VertexBuffer, InMorphVertexBuffer, s.NumBones);
			else
				vf->Init<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::Default>(&VertexBuffer, InMorphVertexBuffer, s.NumBones);
		}
		else
		{
			if (bUseFullPrecisionUVs)
				vf->Init<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::HighPrecision>(&VertexBuffer, InMorphVertexBuffer, s.NumBones);
			else
				vf->Init<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default>(&VertexBuffer, InMorphVertexBuffer, s.NumBones);
		}
		BeginInitResource(vf);
		VertexFactories.Add(vf);
	};

	for (auto& s : Sections)
	{
		if (InPhysics && InFeatureLevel >= ERHIFeatureLevel::ES3_1)
		{
			if (InMorphVertexBuffer)
			{
				if (HasExtraBoneInfluences)
					CreateVertexFactory(s, new FMorphPhysicsExtraInfluencesFurSkinVertexFactory(InFeatureLevel));
				else
					CreateVertexFactory(s, new FMorphPhysicsFurSkinVertexFactory(InFeatureLevel));
			}
			else
			{
				if (HasExtraBoneInfluences)
					CreateVertexFactory(s, new FPhysicsExtraInfluencesFurSkinVertexFactory(InFeatureLevel));
				else
					CreateVertexFactory(s, new FPhysicsFurSkinVertexFactory(InFeatureLevel));
			}
		}
		else
		{
			if (InMorphVertexBuffer)
			{
				if (HasExtraBoneInfluences)
					CreateVertexFactory(s, new FMorphExtraInfluencesFurSkinVertexFactory(InFeatureLevel));
				else
					CreateVertexFactory(s, new FMorphFurSkinVertexFactory(InFeatureLevel));
			}
			else
			{
				if (HasExtraBoneInfluences)
					CreateVertexFactory(s, new FExtraInfluencesFurSkinVertexFactory(InFeatureLevel));
				else
					CreateVertexFactory(s, new FFurSkinVertexFactory(InFeatureLevel));
			}
		}
	}
}

void FFurSkinData::UnbindChangeDelegates()
{
#if WITH_EDITORONLY_DATA
	if (FurSplinesChangeHandle.IsValid())
	{
		FurSplinesAssigned->OnSplinesChanged.Remove(FurSplinesChangeHandle);
		FurSplinesChangeHandle.Reset();
	}
	if (FurSplinesCombHandle.IsValid())
	{
		FurSplinesAssigned->OnSplinesCombed.Remove(FurSplinesCombHandle);
		FurSplinesCombHandle.Reset();
	}
	if (SkeletalMeshChangeHandle.IsValid())
	{
		SkeletalMesh->GetOnMeshChanged().Remove(SkeletalMeshChangeHandle);
		SkeletalMeshChangeHandle.Reset();
	}
	for (int32 i = 0; i < GuideMeshes.Num(); i++)
	{
		if (GuideMeshes[i])
			GuideMeshes[i]->GetOnMeshChanged().Remove(GuideMeshesChangeHandles[i]);
	}
	GuideMeshesChangeHandles.Reset();
#endif // WITH_EDITORONLY_DATA
}

void FFurSkinData::Set(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent)
{
	UnbindChangeDelegates();

	FFurData::Set(InFurLayerCount, InLod, InFurComponent);

	SkeletalMesh = InFurComponent->SkeletalGrowMesh;
	GuideMeshes = InFurComponent->SkeletalGuideMeshes;

	check(SkeletalMesh);

	if (FurSplinesAssigned == NULL && GuideMeshes.Num() > 0)
	{
		FurSplinesUsed = NewObject<UFurSplines>();
		GenerateSplines(FurSplinesUsed, SkeletalMesh, InLod, GuideMeshes);
	}

#if WITH_EDITORONLY_DATA
	SkeletalMeshChangeHandle = SkeletalMesh->GetOnMeshChanged().AddLambda([this]() { BuildFur(BuildType::Full); });
	if (FurSplinesAssigned)
	{
		FurSplinesChangeHandle = FurSplinesAssigned->OnSplinesChanged.AddLambda([this]() { BuildFur(BuildType::Splines); });
		FurSplinesCombHandle = FurSplinesAssigned->OnSplinesCombed.AddLambda([this](const TArray<uint32>& VertexSet) { BuildFur(VertexSet); });
	}
	else if (GuideMeshes.Num() > 0)
	{
		for (const auto& GuideMesh : GuideMeshes)
		{
			if (GuideMesh)
			{
				auto Handle = GuideMesh->GetOnMeshChanged().AddLambda([this, InLod]() {
					if (FurSplinesUsed)
						FurSplinesUsed->ConditionalBeginDestroy();
					FurSplinesUsed = NewObject<UFurSplines>();
					GenerateSplines(FurSplinesUsed, SkeletalMesh, InLod, GuideMeshes);
					BuildFur(BuildType::Splines);
				});
				GuideMeshesChangeHandles.Add(Handle);
			}
			else
			{
				GuideMeshesChangeHandles.Emplace();
			}
		}
	}
#endif // WITH_EDITORONLY_DATA
}

bool FFurSkinData::Compare(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent)
{
	return FFurData::Compare(InFurLayerCount, InLod, InFurComponent) && SkeletalMesh == InFurComponent->SkeletalGrowMesh && GuideMeshes == InFurComponent->SkeletalGuideMeshes;
}

bool FFurSkinData::Similar(int32 InLod, class UGFurComponent* InFurComponent)
{
	return FFurData::Similar(InLod, InFurComponent) && SkeletalMesh == InFurComponent->SkeletalGrowMesh && GuideMeshes == InFurComponent->SkeletalGuideMeshes;
}

void FFurSkinData::BuildFur(BuildType Build)
{
	auto* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	const FSkeletalMeshLODRenderData& LodRenderData = SkeletalMeshResource->LODRenderData[Lod];
	if (LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetUseHighPrecisionTangentBasis())
		BuildFur<EStaticMeshVertexTangentBasisType::HighPrecision>(LodRenderData, Build);
	else
		BuildFur<EStaticMeshVertexTangentBasisType::Default>(LodRenderData, Build);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, BuildType Build)
{
	if (LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetUseFullPrecisionUVs())
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::HighPrecision>(LodRenderData, Build);
	else
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::Default>(LodRenderData, Build);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, BuildType Build)
{
	if (LodRenderData.SkinWeightVertexBuffer.HasExtraBoneInfluences())
		BuildFur<TangentBasisTypeT, UVTypeT, true>(LodRenderData, Build);
	else
		BuildFur<TangentBasisTypeT, UVTypeT, false>(LodRenderData, Build);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT, bool bExtraBoneInfluencesT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, BuildType Build)
{
	typedef FFurSkinVertex<TangentBasisTypeT, UVTypeT, bExtraBoneInfluencesT> VertexType;

	bUseHighPrecisionTangentBasis = TangentBasisTypeT == EStaticMeshVertexTangentBasisType::HighPrecision;
	bUseFullPrecisionUVs = UVTypeT == EStaticMeshVertexUVType::HighPrecision;
	HasExtraBoneInfluences = bExtraBoneInfluencesT;

	const auto& SourcePositions = LodRenderData.StaticVertexBuffers.PositionVertexBuffer;
	const auto& SourceSkinWeights = LodRenderData.SkinWeightVertexBuffer;
	const auto& SourceVertices = LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer;
	const auto& SourceColors = LodRenderData.StaticVertexBuffers.ColorVertexBuffer;

	const uint32 SourceVertexCount = SourcePositions.GetNumVertices();
	check(SourceVertexCount == SourceSkinWeights.GetNumVertices() && SourceVertexCount == SourceVertices.GetNumVertices());

	bool HasVertexColor = SourceColors.GetNumVertices() > 0;
	check(!HasVertexColor || SourceVertexCount == SourceColors.GetNumVertices());

	if (Build == BuildType::Full)
	{
		UnpackNormals<TangentBasisTypeT>(SourceVertices);
	}
	if (Build >= BuildType::Splines)
		GenerateSplineMap(SourcePositions);

	uint32 NewVertexCount = VertexCountPerLayer * FurLayerCount;

	while (RenderThreadDataSubmissionPending)
		;

	TArray<FSection>& LocalSections = Sections.Num() ? TempSections : Sections;
	LocalSections.SetNum(LodRenderData.RenderSections.Num());

	FFurSkinVertexBlitter<TangentBasisTypeT, UVTypeT, bExtraBoneInfluencesT> VertexBlitter(SourcePositions, SourceVertices, SourceColors, SourceSkinWeights);

	VertexType* Vertices = VertexBuffer.Lock<VertexType>(NewVertexCount);
	uint32 SectionVertexOffset = 0;
	float MaxDistSq = 0.0f;
	for (int32 SectionIndex = 0; SectionIndex < LodRenderData.RenderSections.Num(); SectionIndex++)
	{
		const auto& SourceSection = LodRenderData.RenderSections[SectionIndex];
		FSection& FurSection = LocalSections[SectionIndex];

		FurSection.MinVertexIndex = SectionVertexOffset;

		uint32 VertCount = GenerateFurVertices(SourceSection.BaseVertexIndex, SourceSection.BaseVertexIndex + SourceSection.NumVertices, Vertices + SectionVertexOffset, VertexBlitter);
		if (Build == BuildType::Full)
		{
			const auto& RefPose = SkeletalMesh->RefSkeleton.GetRawRefBonePose();
			for (uint32 i = 0; i < VertCount; i++)
			{
				uint32 VertexIndex = SectionVertexOffset + i;
				for (uint32 b = 0; b < VertexType::NumInfluences; b++)
				{
					if (Vertices[VertexIndex].InfluenceWeights[b] == 0)
						break;
					uint32 BoneIndex = SourceSection.BoneMap[Vertices[VertexIndex].InfluenceBones[b]];
					float distSq = FVector::DistSquared(Vertices[VertexIndex].Position, RefPose[BoneIndex].GetTranslation());
					if (distSq > MaxDistSq)
						MaxDistSq = distSq;
				}
			}
		}
		SectionVertexOffset += VertCount * FurLayerCount;

		FurSection.MaxVertexIndex = SectionVertexOffset - 1;
	}
	VertexBuffer.Unlock();
	if (Build == BuildType::Full)
		MaxVertexBoneDistance = sqrtf(MaxDistSq);

	if (Build >= BuildType::Splines || FurLayerCount != OldFurLayerCount || RemoveFacesWithoutSplines != OldRemoveFacesWithoutSplines)
	{
		OldFurLayerCount = FurLayerCount;
		OldRemoveFacesWithoutSplines = RemoveFacesWithoutSplines;

		// indices
		TArray<uint32> SourceIndices;
		LodRenderData.MultiSizeIndexContainer.GetIndexBuffer(SourceIndices);

		auto& Indices = IndexBuffer.Lock();
		Indices.Reset();
		Indices.AddUninitialized(SourceIndices.Num() * FurLayerCount);
		uint32 Idx = 0;
		for (int32 SectionIndex = 0; SectionIndex < LodRenderData.RenderSections.Num(); SectionIndex++)
		{
			const auto& SourceSection = LodRenderData.RenderSections[SectionIndex];
			FSection& FurSection = LocalSections[SectionIndex];

			FurSection.MaterialIndex = SourceSection.MaterialIndex;
			FurSection.BaseIndex = Idx;

			for (int32 Layer = 0; Layer < FurLayerCount; Layer++)
			{
				int32 VertexIndexOffset = Layer * ((FurSection.MaxVertexIndex - FurSection.MinVertexIndex + 1) / FurLayerCount) + FurSection.MinVertexIndex;
				check(VertexIndexOffset >= 0);
				if (FurSplinesUsed && RemoveFacesWithoutSplines)
				{
					for (uint32 t = 0; t < SourceSection.NumTriangles; ++t)
					{
						uint32 Idx0 = SourceIndices[SourceSection.BaseIndex + t * 3];
						uint32 Idx1 = SourceIndices[SourceSection.BaseIndex + t * 3 + 1];
						uint32 Idx2 = SourceIndices[SourceSection.BaseIndex + t * 3 + 2];
						if (SplineMap[Idx0] >= 0 && SplineMap[Idx1] >= 0 && SplineMap[Idx2] >= 0)
						{
							Indices[Idx++] = VertexRemap[Idx0] + VertexIndexOffset;
							Indices[Idx++] = VertexRemap[Idx1] + VertexIndexOffset;
							Indices[Idx++] = VertexRemap[Idx2] + VertexIndexOffset;
						}
					}
				}
				else
				{
					VertexIndexOffset -= SourceSection.BaseVertexIndex;
					for (uint32 i = 0; i < SourceSection.NumTriangles * 3; ++i)
						Indices[Idx++] = SourceIndices[SourceSection.BaseIndex + i] + VertexIndexOffset;
				}
			}
			FurSection.NumTriangles = (Idx - FurSection.BaseIndex) / 3;
			FurSection.NumBones = SourceSection.BoneMap.Num();
		}
		check(Idx <= (uint32)Indices.Num());
		Indices.RemoveAt(Idx, Indices.Num() - Idx, false);
		IndexBuffer.Unlock();

		if (TempSections.Num())
		{
			ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this, NewVertexCount](FRHICommandListImmediate& RHICmdList) {
				Sections = TempSections;
				VertexCount = NewVertexCount;
			});
		}
		else
		{
			VertexCount = NewVertexCount;
		}
	}

	RenderThreadDataSubmissionPending = true;
	ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this](FRHICommandListImmediate& RHICmdList) {
		RenderThreadDataSubmissionPending = false;
	});

#if !WITH_EDITORONLY_DATA
	Normals.SetNum(0, true);
	SplineMap.SetNum(0, true);
	VertexRemap.SetNum(0, true);
#endif // WITH_EDITORONLY_DATA
}

void FFurSkinData::BuildFur(const TArray<uint32>& InVertexSet)
{
	auto* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	const FSkeletalMeshLODRenderData& LodRenderData = SkeletalMeshResource->LODRenderData[Lod];
	if (LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetUseHighPrecisionTangentBasis())
		BuildFur<EStaticMeshVertexTangentBasisType::HighPrecision>(LodRenderData, InVertexSet);
	else
		BuildFur<EStaticMeshVertexTangentBasisType::Default>(LodRenderData, InVertexSet);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, const TArray<uint32>& InVertexSet)
{
	if (LodRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetUseFullPrecisionUVs())
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::HighPrecision>(LodRenderData, InVertexSet);
	else
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::Default>(LodRenderData, InVertexSet);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, const TArray<uint32>& InVertexSet)
{
	if (LodRenderData.SkinWeightVertexBuffer.HasExtraBoneInfluences())
		BuildFur<TangentBasisTypeT, UVTypeT, true>(LodRenderData, InVertexSet);
	else
		BuildFur<TangentBasisTypeT, UVTypeT, false>(LodRenderData, InVertexSet);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT, bool bExtraBoneInfluencesT>
inline void FFurSkinData::BuildFur(const FSkeletalMeshLODRenderData& LodRenderData, const TArray<uint32>& InVertexSet)
{
	typedef FFurSkinVertex<TangentBasisTypeT, UVTypeT, bExtraBoneInfluencesT> VertexType;

	while (RenderThreadDataSubmissionPending)
		;

	const auto& SrcSections = LodRenderData.RenderSections;
	uint32 SectionIndex = 0;
	uint32 SectionCount = SrcSections.Num();
	uint32 SectionVertexIndexBegin = SrcSections[SectionIndex].BaseVertexIndex;
	uint32 SectionVertexIndexEnd = SectionVertexIndexBegin + SrcSections[SectionIndex].NumVertices;

	const auto& LocalSections = TempSections.Num() ? TempSections : Sections;
	uint32 DstSectionVertexBegin = LocalSections[SectionIndex].MinVertexIndex;
	uint32 DstSectionVertexCountPerLayer = (LocalSections[SectionIndex].MaxVertexIndex + 1 - DstSectionVertexBegin) / FurLayerCount;

	VertexType* Vertices = VertexBuffer.Lock<VertexType>(VertexCountPerLayer * FurLayerCount);
	bool UseRemap = VertexRemap.Num() > 0;
	for (int32 Layer = 0; Layer < FurLayerCount; Layer++)
	{
		auto GenLayerData = CalcFurGenLayerData(FurLayerCount - Layer);
		for (uint32 SrcVertexIndex : InVertexSet)
		{
			uint32 checkCounter = 0;
			while (SrcVertexIndex < SectionVertexIndexBegin || SrcVertexIndex >= SectionVertexIndexEnd)
			{
				SectionIndex = (SectionIndex + 1) % SectionCount;
				SectionVertexIndexBegin = SrcSections[SectionIndex].BaseVertexIndex;
				SectionVertexIndexEnd = SrcSections[SectionIndex].BaseVertexIndex + SrcSections[SectionIndex].NumVertices;
				DstSectionVertexBegin = LocalSections[SectionIndex].MinVertexIndex;
				DstSectionVertexCountPerLayer = (LocalSections[SectionIndex].MaxVertexIndex + 1 - DstSectionVertexBegin) / FurLayerCount;
				check(checkCounter++ < SectionCount);
			}
			uint32 DstVertexIndex = UseRemap ? VertexRemap[SrcVertexIndex] : SrcVertexIndex - SectionVertexIndexBegin;
			DstVertexIndex += DstSectionVertexCountPerLayer * Layer + DstSectionVertexBegin;
			VertexType& Vertex = Vertices[DstVertexIndex];

			if (FurSplinesUsed)
			{
				int32 SplineIndex = SplineMap[SrcVertexIndex];
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Normals[SrcVertexIndex], GenLayerData, SplineIndex);
			}
			else
			{
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Normals[SrcVertexIndex], GenLayerData);
			}
		}
	}

	VertexBuffer.Unlock();

	RenderThreadDataSubmissionPending = true;
	ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this](FRHICommandListImmediate& RHICmdList) {
		RenderThreadDataSubmissionPending = false;
	});
}

/** Generate Splines */
void GenerateSplines(UFurSplines* Splines, USkeletalMesh* InSkeletalMesh, int32 InLod, const TArray<USkeletalMesh*>& InGuideMeshes)
{
	auto* SkeletalMeshResource = InSkeletalMesh->GetResourceForRendering();
	check(SkeletalMeshResource);

	if (InLod >= SkeletalMeshResource->LODRenderData.Num())
		InLod = SkeletalMeshResource->LODRenderData.Num() - 1;
	const auto& LodModel = SkeletalMeshResource->LODRenderData[InLod];

	const auto& SourcePositions = LodModel.StaticVertexBuffers.PositionVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	int32 ControlPointCount = InGuideMeshes.Num() + 1;
	Splines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	Splines->ControlPointCount = ControlPointCount;
	for (uint32 i = 0; i < VertexCount; i++)
	{
		int32 Index = i * ControlPointCount;
		Splines->Vertices[Index] = SourcePositions.VertexPosition(i);
	}

	int32 k = 1;
	for (USkeletalMesh* GuideMesh : InGuideMeshes)
	{
		if (GuideMesh)
		{
			auto* SkeletalMeshResource2 = GuideMesh->GetResourceForRendering();
			check(SkeletalMeshResource2);
			const auto& LodModel2 = SkeletalMeshResource2->LODRenderData[InLod];
			const auto& SourcePositions2 = LodModel2.StaticVertexBuffers.PositionVertexBuffer;
			int32 c = FMath::Min(SourcePositions2.GetNumVertices(), VertexCount);
			for (int32 i = 0; i < c; i++)
				Splines->Vertices[i * ControlPointCount + k] = SourcePositions2.VertexPosition(i);
			for (uint32 i = c; i < VertexCount; i++)
				Splines->Vertices[i * ControlPointCount + k] = Splines->Vertices[i * ControlPointCount + (k - 1)];
		}
		else
		{
			for (uint32 i = 0; i < VertexCount; i++)
				Splines->Vertices[i * ControlPointCount + k] = Splines->Vertices[i * ControlPointCount + (k - 1)];
		}
		k++;
	}
}
