// Copyright 2017 GiM s.r.o. All Rights Reserved.

#include "FurComponent.h"
#include "GFur.h"
#include "FurSplines.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"
#include "Runtime/Engine/Public/DynamicMeshBuilder.h"
#include "Runtime/Engine/Public/GPUSkinVertexFactory.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Launch/Resources/Version.h"
#include "ShaderParameterUtils.h"

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
struct FFurSkinVertex: FSoftSkinVertex
{
	FVector FurOffset;
};

/** Vertex Buffer */
class FFurVertexBuffer : public FVertexBuffer
{
public:
	TArray<FFurSkinVertex> Vertices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FFurSkinVertex), BUF_Static, CreateInfo);
		// Copy the vertex data into the vertex buffer.
		void* VertexBufferData =
			RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FFurSkinVertex), RLM_WriteOnly);
		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FFurSkinVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

/** Index Buffer */
class FFurIndexBuffer : public FIndexBuffer
{
public:
	TArray<int32> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(int32), Indices.Num() * sizeof(int32), BUF_Static, CreateInfo);
		// Write the indices to the index buffer.
		void* Buffer = RHILockIndexBuffer(IndexBufferRHI, 0, Indices.Num() * sizeof(int32), RLM_WriteOnly);
		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(int32));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}
};

unsigned int MaxGPUSkinBones = 256;

class FFurVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
public:
	void Bind(const FShaderParameterMap& ParameterMap) override
	{
		MeshOriginParameter.Bind(ParameterMap, TEXT("MeshOrigin"));
		MeshExtensionParameter.Bind(ParameterMap, TEXT("MeshExtension"));
		FurOffsetPowerParameter.Bind(ParameterMap, TEXT("FurOffsetPower"));
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
	FShaderResourceParameter BoneMatrices;
	FShaderResourceParameter PreviousBoneMatrices;
	FShaderResourceParameter BoneFurOffsets;
	FShaderResourceParameter PreviousBoneFurOffsets;
};

/** Vertex Factory */
class FFurVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FFurVertexFactory);

public:
	struct FShaderDataType
	{
		FShaderDataType(ERHIFeatureLevel::Type InFeatureLevel)
			: CurrentBuffer(0)
			, PreviousFrameNumber(0)
			, CurrentFrameNumber(0)
			, MeshOrigin(0, 0, 0)
			, MeshExtension(1, 1, 1)
			, FurOffsetPower(2.0f)
			, FeatureLevel(InFeatureLevel)
		{
		}

		/** Mesh origin and Mesh Extension for Mesh compressions **/
		/** This value will be (0, 0, 0), (1, 1, 1) relatively for non compressed meshes **/
		FVector MeshOrigin;
		FVector MeshExtension;
		float FurOffsetPower;

		void Init(uint32 InBoneCount)
		{
			BoneCount = InBoneCount;
		}

		// @param FrameTime from GFrameTime
		void UpdateBoneData(const TArray<FMatrix>& ReferenceToLocalMatrices, const TArray<FVector>& LinearOffsets, const TArray<FVector>& AngularOffsets,
			const TArray<FMatrix>& LastTransformations, const TArray<FBoneIndexType>& BoneMap, uint32 FrameNumber, ERHIFeatureLevel::Type FeatureLevel);

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
		const FVertexBufferAndSRV& GetBoneBufferForReading(bool bPrevious, uint32 FrameNumber) const
		{
			const FVertexBufferAndSRV* RetPtr = &GetBoneBufferInternal(bPrevious, FrameNumber);

			if (!RetPtr->VertexBufferRHI.IsValid())
			{
				// this only should happen if we request the old data
				check(bPrevious);

				// if we don't have any old data we use the current one
				RetPtr = &GetBoneBufferInternal(false, FrameNumber);

				// at least the current one needs to be valid when reading
				check(RetPtr->VertexBufferRHI.IsValid());
			}

			return *RetPtr;
		}

		// @param bPrevious true:previous, false:current
		// @param FrameNumber usually from View.Family->FrameNumber
		// @return IsValid() can fail, then you have to create the buffers first (or if the size changes)
		FVertexBufferAndSRV& GetBoneBufferForWriting(bool bPrevious, uint32 FrameNumber)
		{
			const FShaderDataType* This = (const FShaderDataType*)this;

			// non const version maps to const version
			return (FVertexBufferAndSRV&)This->GetBoneBufferInternal(bPrevious, FrameNumber);
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
		// from GFrameNumber, to detect pause and old data when an object was not rendered for some time
		uint32 PreviousFrameNumber;
		uint32 CurrentFrameNumber;
		// if FeatureLevel < ERHIFeatureLevel::ES3_1
		FUniformBufferRHIRef UniformBuffer;
		uint32 BoneCount;
		ERHIFeatureLevel::Type FeatureLevel;

		void GoToNextFrame(uint32 FrameNumber);

		// to support GetBoneBufferForWriting() and GetBoneBufferForReading()
		// @param bPrevious true:previous, false:current
		// @param FrameNumber usually from View.Family->FrameNumber
		// @return might not pass the IsValid() 
		const FVertexBufferAndSRV& GetBoneBufferInternal(bool bPrevious, uint32 FrameNumber) const
		{
			check(IsInParallelRenderingThread());

			// This test prevents skeletal meshes keeping velocity when we pause (e.g. simulate pause)
			// CurrentFrameNumber <= FrameNumber which means non-sequential frames are also skipped 
			if ((FrameNumber - PreviousFrameNumber) > 1)
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

			// This test prevents skeletal meshes keeping velocity when we pause (e.g. simulate pause)
			// CurrentFrameNumber <= FrameNumber which means non-sequential frames are also skipped 
			if ((CurrentFrameNumber - PreviousFrameNumber) > 1)
			{
				bPrevious = false;
			}

			uint32 BufferIndex = CurrentBuffer ^ (uint32)bPrevious;

			const FVertexBufferAndSRV& Ret = BoneFurOffsetsBuffer[BufferIndex];
			return Ret;
		}
	};

	FFurVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FVertexFactory(InFeatureLevel)
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
		FVertexStreamComponent BoneWeights;
		FVertexStreamComponent FurOffset;
	};

	void Init(const FFurVertexBuffer* VertexBuffer, uint32 BoneCount)
	{
		ShaderData.Init(BoneCount);
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(InitProceduralMeshVertexFactory,
			FFurVertexFactory*,
			VertexFactory,
			this,
			const FFurVertexBuffer*,
			VertexBuffer,
			VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				FDataType NewData;
				NewData.PositionComponent =
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurSkinVertex, Position, VET_Float3);
				int c = sizeof(FFurSkinVertex::UVs) / sizeof(FFurSkinVertex::UVs[0]);
				for (int i = 0; i < c; ++i)
				{
					NewData.TextureCoordinates.Add(FVertexStreamComponent(
						VertexBuffer, STRUCT_OFFSET(FFurSkinVertex, UVs[i]), sizeof(FFurSkinVertex), VET_Float2));
				}
				NewData.TangentBasisComponents[0] =
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurSkinVertex, TangentX, VET_PackedNormal);
				NewData.TangentBasisComponents[1] =
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurSkinVertex, TangentZ, VET_PackedNormal);
				NewData.ColorComponent =
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurSkinVertex, Color, VET_Color);
				NewData.BoneIndices = FVertexStreamComponent(
					VertexBuffer, STRUCT_OFFSET(FFurSkinVertex, InfluenceBones), sizeof(FFurSkinVertex), VET_UByte4);
				NewData.BoneWeights = FVertexStreamComponent(VertexBuffer,
					STRUCT_OFFSET(FFurSkinVertex, InfluenceWeights),
					sizeof(FFurSkinVertex),
					VET_UByte4N);
				NewData.FurOffset =
					STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurSkinVertex, FurOffset, VET_Float3);

				VertexFactory->SetData(NewData);
			});
	}

	static FVertexFactoryShaderParameters* ConstructShaderParameters(EShaderFrequency ShaderFrequency)
	{
		return (ShaderFrequency == SF_Vertex) ? new FFurVertexFactoryShaderParameters() : NULL;
	}


	static bool ShouldCache(EShaderPlatform Platform,
		const class FMaterial* Material,
		const class FShaderType* ShaderType)
	{
		return (Material->IsUsedWithSkeletalMesh() || Material->IsSpecialEngineMaterial());
	}

	static bool ShouldCompilePermutation(EShaderPlatform Platform, const class FMaterial* Material, const FShaderType* ShaderType)
	{
		return (Material->IsUsedWithSkeletalMesh() || Material->IsSpecialEngineMaterial());
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

	FDataType Data;
	FShaderDataType ShaderData;
};

IMPLEMENT_VERTEX_FACTORY_TYPE(FFurVertexFactory, "/Plugin/gFur/Private/GFurFactory.ush", true, false, true, true, false);
#if WITH_EDITORONLY_DATA
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FBoneMatricesUniformShaderParameters, TEXT("Bones"));
#endif // WITH_EDITORONLY_DATA

static FBoneMatricesUniformShaderParameters GBoneUniformStruct;

void FFurVertexFactory::FShaderDataType::GoToNextFrame(uint32 FrameNumber)
{
	PreviousFrameNumber = CurrentFrameNumber;
	CurrentFrameNumber = FrameNumber;
	CurrentBuffer = 1 - CurrentBuffer;
}

void FFurVertexFactory::FShaderDataType::UpdateBoneData(const TArray<FMatrix>& ReferenceToLocalMatrices, const TArray<FVector>& LinearOffsets, const TArray<FVector>& AngularOffsets,
	const TArray<FMatrix>& LastTransformations, const TArray<FBoneIndexType>& BoneMap, uint32 FrameNumber, ERHIFeatureLevel::Type InFeatureLevel)
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
		GoToNextFrame(FrameNumber);

		CurrentBoneBuffer = &GetBoneBufferForWriting(false, FrameNumber);

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

void FFurVertexFactory::FShaderDataType::InitDynamicRHI()
{
	const uint32 NumBones = BoneCount;
	check(NumBones <= MaxGPUSkinBones);
	FSkinMatrix3x4* ChunkMatrices = nullptr;

	FVertexBufferAndSRV* CurrentBoneBuffer = 0;

	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		check(IsInRenderingThread());

		CurrentBoneBuffer = &GetBoneBufferForWriting(false, 0);

		uint32 NumVectors = NumBones * 3;
		check(NumVectors <= (MaxGPUSkinBones * 3));
		uint32 VectorArraySize = NumVectors * sizeof(FVector4);

		if (!IsValidRef(*CurrentBoneBuffer))
		{
			FVertexBufferAndSRV Buffer;
			FRHIResourceCreateInfo CreateInfo;
			Buffer.VertexBufferRHI = RHICreateVertexBuffer(VectorArraySize, (BUF_Dynamic | BUF_ShaderResource), CreateInfo);
			Buffer.VertexBufferSRV = RHICreateShaderResourceView(Buffer.VertexBufferRHI, sizeof(FVector4), PF_A32B32G32R32F);
			*CurrentBoneBuffer = Buffer;
			check(IsValidRef(*CurrentBoneBuffer));
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


void FFurVertexFactoryShaderParameters::SetMesh(FRHICommandList& RHICmdList, FShader* Shader, const FVertexFactory* VertexFactory, const FSceneView& View, const FMeshBatchElement& BatchElement, uint32 DataFlags) const
{
	FRHIVertexShader* ShaderRHI = Shader->GetVertexShader();

	if (ShaderRHI)
	{
		FFurVertexFactory::FShaderDataType& ShaderData = ((FFurVertexFactory*)VertexFactory)->ShaderData;

		SetShaderValue(RHICmdList, ShaderRHI, MeshOriginParameter, ShaderData.MeshOrigin);
		SetShaderValue(RHICmdList, ShaderRHI, MeshExtensionParameter, ShaderData.MeshExtension);
		SetShaderValue(RHICmdList, ShaderRHI, FurOffsetPowerParameter, ShaderData.FurOffsetPower);

		const auto FeatureLevel = View.GetFeatureLevel();
		const auto FrameNumber = View.Family->FrameNumber;

		if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
		{
			if (BoneMatrices.IsBound())
			{
				FShaderResourceViewRHIParamRef CurrentData = ShaderData.GetBoneBufferForReading(false, FrameNumber).VertexBufferSRV;
				RHICmdList.SetShaderResourceViewParameter(ShaderRHI, BoneMatrices.GetBaseIndex(), CurrentData);
			}
			if (PreviousBoneMatrices.IsBound())
			{
				// todo: Maybe a check for PreviousData!=CurrentData would save some performance (when objects don't have velocty yet) but removing the bool also might save performance

				FShaderResourceViewRHIParamRef PreviousData = ShaderData.GetBoneBufferForReading(true, FrameNumber).VertexBufferSRV;
				RHICmdList.SetShaderResourceViewParameter(ShaderRHI, PreviousBoneMatrices.GetBaseIndex(), PreviousData);
			}
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

/** Fur Data */
struct FFurData
{
	struct FSection
	{
		uint32 BaseIndex;
		uint32 NumTriangles;
		uint32 BaseVertexIndex;
		int32 NumVertices;
		int32 NumBones;
	};

	USkeletalMesh* SkeletalMesh;
	UFurSplines* FurSplines;
	TArray<USkeletalMesh*> GuideMeshes;
	FFurVertexBuffer VertexBuffer;
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

	FFurData(USkeletalMesh* InSkeletalMesh, int InLod, UFurSplines* InFurSplines, const TArray<USkeletalMesh*>& InGuideMeshes, int InFurLayerCount,
		float InFurLength, float InMinFurLength, float InShellBias, float InHairLengthForceUniformity, float InNoiseStrength)
	{
		SkeletalMesh = InSkeletalMesh;
		FurLayerCount = InFurLayerCount;
		FurSplines = InFurSplines;
		GuideMeshes = InGuideMeshes;
		Lod = InLod;
		FurLength = InFurLength;
		ShellBias = InShellBias;
		HairLengthForceUniformity = InHairLengthForceUniformity;
		MinFurLength = InMinFurLength;
		CurrentMinFurLength = InFurLength;
		CurrentMaxFurLength = InFurLength;
		NoiseStrength = InNoiseStrength;

		if (InFurSplines == nullptr && InGuideMeshes.Num() > 0)
			InFurSplines = generateSplines(InSkeletalMesh, InLod, InGuideMeshes);

		auto* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
		check(SkeletalMeshResource);

		TArray<uint32> Indices;
		TArray<int32> SplineMap;
		TArray<uint32> VertexSub;

		const auto& LodModel = SkeletalMeshResource->LODRenderData[InLod];
		const auto& SourcePositions = LodModel.StaticVertexBuffers.PositionVertexBuffer;
		const auto& SourceSkinWeights = LodModel.SkinWeightVertexBuffer;
		const auto& SourceVertices = LodModel.StaticVertexBuffers.StaticMeshVertexBuffer;
		const auto& SourceColors = LodModel.StaticVertexBuffers.ColorVertexBuffer;
		LodModel.MultiSizeIndexContainer.GetIndexBuffer(Indices);

		check(SourcePositions.GetNumVertices() == SourceSkinWeights.GetNumVertices() && SourcePositions.GetNumVertices() == SourceVertices.GetNumVertices());

		uint32 NumTexCoords = SourceVertices.GetNumTexCoords();
		bool hasVertexColor = SourceColors.GetNumVertices() > 0;
		check(!hasVertexColor || SourcePositions.GetNumVertices() == SourceColors.GetNumVertices());

		int BoneCount = InSkeletalMesh->RefBasesInvMatrix.Num();
		TArray<FMatrix> Bones;
		Bones.AddUninitialized(BoneCount);
		for (int i = 0; i < BoneCount; i++)
			Bones[i] = InSkeletalMesh->GetRefPoseMatrix(i);
		float MaxDistSq = 0.0f;
		uint32 influencesCount = SourceSkinWeights.GetStride() / 2;
		if (SourceSkinWeights.HasExtraBoneInfluences())
		{
			for (int i = 0, c = SourcePositions.GetNumVertices(); i < c; i++)
			{
				const auto& WeightInfo = *SourceSkinWeights.GetSkinWeightPtr<true>(i);
				for (uint32 b = 0; b < influencesCount; b++)
				{
					if (WeightInfo.InfluenceWeights[b] == 0)
						break;
					float distSq = FVector::DistSquared(SourcePositions.VertexPosition(i), Bones[WeightInfo.InfluenceBones[b]].GetOrigin());
					if (distSq > MaxDistSq)
						MaxDistSq = distSq;
				}
			}
		}
		else
		{
			for (int i = 0, c = SourcePositions.GetNumVertices(); i < c; i++)
			{
				const auto& WeightInfo = *SourceSkinWeights.GetSkinWeightPtr<false>(i);
				for (uint32 b = 0; b < influencesCount; b++)
				{
					if (WeightInfo.InfluenceWeights[b] == 0)
						break;
						float distSq = FVector::DistSquared(SourcePositions.VertexPosition(i), Bones[WeightInfo.InfluenceBones[b]].GetOrigin());
					if (distSq > MaxDistSq)
						MaxDistSq = distSq;
				}
			}
		}
		MaxVertexBoneDistance = sqrtf(MaxDistSq);

		FMatrix deltaMatrix;
		if (InFurSplines)
		{
			float MinLen = FLT_MAX;
			float MaxLen = -FLT_MAX;
			SplineMap.Reserve(SourcePositions.GetNumVertices());
			for (uint32 i = 0; i < SourcePositions.GetNumVertices(); i++)
			{
				int ClosestSplineIndex = -1;
				float ClosestDist = FLT_MAX;
				for (int si = 0; si < InFurSplines->Index.Num(); si++)
				{
					FVector p = InFurSplines->Vertices[InFurSplines->Index[si]];
					p.Y = -p.Y;
					const auto& Position = SourcePositions.VertexPosition(i);
					float d = FVector::DistSquared(p, Position);
					if (d < ClosestDist)
					{
						ClosestDist = d;
						ClosestSplineIndex = si;
					}
				}
				if (ClosestDist < 0.01f)
				{
					uint32 idx = InFurSplines->Index[ClosestSplineIndex];
					FVector p1 = InFurSplines->Vertices[idx];
					FVector p2 = InFurSplines->Vertices[idx + InFurSplines->Count[ClosestSplineIndex] - 1];
					FVector normal = SourceVertices.VertexTangentZ(i);
					normal.Y = -normal.Y;
					if (FVector::DotProduct(p2 - p1, normal) > 0.0f || MinFurLength > 0.0f)
					{
						float l = (p2 - p1).Size();
						if (l < MinLen)
							MinLen = l;
						if (l > MaxLen)
							MaxLen = l;
						SplineMap.Add(ClosestSplineIndex);
					}
					else
					{
						SplineMap.Add(-1);
					}
				}
				else
				{
					SplineMap.Add(-1);
				}
			}
			CurrentMinFurLength = MinLen * InFurLength;
			if (CurrentMinFurLength < MinFurLength)
				CurrentMinFurLength = MinFurLength;
			CurrentMaxFurLength = MaxLen * InFurLength;
		}


		int IndexOffset = 0;
		for (int SectionIndex = 0; SectionIndex < LodModel.RenderSections.Num(); SectionIndex++)
		{
			const auto& ModelSection = LodModel.RenderSections[SectionIndex];
			FSection& FurSection = Sections[Sections.AddUninitialized()];
			new (&FurSection) FSection();
			FurSection.BaseVertexIndex = VertexBuffer.Vertices.Num();
			FurSection.BaseIndex = IndexBuffer.Indices.Num();

			if (InFurSplines)
			{
				VertexSub.Reset(ModelSection.NumVertices);
				VertexSub.AddUninitialized(ModelSection.NumVertices);
				uint32* Vert = &VertexSub[0];
				uint32 Count = 0;
				for (uint32 i = 0; i < ModelSection.NumVertices; ++i)
				{
					int Index = SplineMap[ModelSection.BaseVertexIndex + i];
					if (Index < 0)
						Count++;
					Vert[i] = Count;
				}
			}

			for (int Layer = FurLayerCount; Layer > 0; --Layer)
			{
				float LinearFactor = Layer / (float)FurLayerCount;
				float NonLinearFactor;
				float Derivative;
				if (ShellBias > 0.0f)
				{
					float invShellBias = 1.0f / ShellBias;
					NonLinearFactor = LinearFactor / (LinearFactor + invShellBias) * (1.0f + invShellBias);
					Derivative = (invShellBias + invShellBias * invShellBias) / FMath::Square(LinearFactor + invShellBias);
				}
				else
				{
					NonLinearFactor = LinearFactor;
					Derivative = 1.0f;
				}
				for (uint32 i = 0; i < ModelSection.NumVertices; ++i)
				{
					FFurSkinVertex Vert;
					uint32 SourceVertexIndex = ModelSection.BaseVertexIndex + i;
					Vert.Position = SourcePositions.VertexPosition(SourceVertexIndex);
					Vert.TangentX = SourceVertices.VertexTangentX(SourceVertexIndex);
					Vert.TangentY = SourceVertices.VertexTangentY(SourceVertexIndex);
					Vert.TangentZ = SourceVertices.VertexTangentZ(SourceVertexIndex);
					for (uint32 tc = 0; tc < NumTexCoords; tc++)
						Vert.UVs[tc] = SourceVertices.GetVertexUV(SourceVertexIndex, tc);
					Vert.Color = hasVertexColor ? SourceColors.VertexColor(SourceVertexIndex) : FColor(255, 255, 255, 255);
					uint32 ib = 0;
					if (SourceSkinWeights.HasExtraBoneInfluences())
					{
						const auto& WeightInfo = *SourceSkinWeights.GetSkinWeightPtr<true>(i);
						for (ib = 0; ib < influencesCount; ib++)
						{
							Vert.InfluenceBones[ib] = WeightInfo.InfluenceBones[ib];
							Vert.InfluenceWeights[ib] = WeightInfo.InfluenceWeights[ib];
						}
					}
					else
					{
						const auto& WeightInfo = *SourceSkinWeights.GetSkinWeightPtr<false>(i);
						for (ib = 0; ib < influencesCount; ib++)
						{
							Vert.InfluenceBones[ib] = WeightInfo.InfluenceBones[ib];
							Vert.InfluenceWeights[ib] = WeightInfo.InfluenceWeights[ib];
						}
					}
					for (; ib < MAX_TOTAL_INFLUENCES; ib++)
					{
						Vert.InfluenceBones[ib] = 0;
						Vert.InfluenceWeights[ib] = 0;
					}

					if (InFurSplines)
					{
						int Index = SplineMap[ModelSection.BaseVertexIndex + i];
						if (Index >= 0)
						{
							int Beginning = InFurSplines->Index[Index];
							int Count = InFurSplines->Count[Index];

							float Bias = NonLinearFactor * (Count - 1);
							int Bottom = (int)Bias;
							int Top = (int)ceilf(Bias);
							float Height = Bias - Bottom;

							FVector Spline = InFurSplines->Vertices[Beginning + Count - 1] - InFurSplines->Vertices[Beginning];
							float SplineLength = Spline.Size() * InFurLength;
							FVector TangentZ = Vert.TangentZ;
							FVector Normal = TangentZ;
							Normal.Y = -Normal.Y;
							if (FVector::DotProduct(Normal, Spline) <= 0.0f)
							{
								Vert.FurOffset = TangentZ * (NonLinearFactor * MinFurLength);
							}
							else if (SplineLength < MinFurLength)
							{
								if (SplineLength >= 0.0001f)
								{
									float k = MinFurLength / SplineLength;
									Vert.FurOffset = InFurSplines->Vertices[Beginning + Bottom] * (1.0f - Height) + InFurSplines->Vertices[Beginning + Top] * Height;
									Vert.FurOffset = (Vert.FurOffset - InFurSplines->Vertices[Beginning]) * InFurLength * k + InFurSplines->Vertices[Beginning];
									Vert.FurOffset.Y = -Vert.FurOffset.Y;
									Vert.FurOffset = Vert.FurOffset - Vert.Position;
								}
								else
								{
									Vert.FurOffset = TangentZ * (NonLinearFactor * MinFurLength);
								}
								SplineLength = MinFurLength;
							}
							else
							{
								Vert.FurOffset = InFurSplines->Vertices[Beginning + Bottom] * (1.0f - Height) + InFurSplines->Vertices[Beginning + Top] * Height;
								Vert.FurOffset = (Vert.FurOffset - InFurSplines->Vertices[Beginning]) * InFurLength + InFurSplines->Vertices[Beginning];
								Vert.FurOffset.Y = -Vert.FurOffset.Y;
								Vert.FurOffset = Vert.FurOffset - Vert.Position;
							}
							float r = FMath::RandRange(-InNoiseStrength * Derivative, InNoiseStrength * Derivative);
							Vert.FurOffset += TangentZ * r;

							Vert.UVs[1].X = Vert.FurOffset.Size();
							Vert.UVs[1].Y = NonLinearFactor;
							Vert.UVs[2].X = LinearFactor;
							Vert.UVs[2].Y = SplineLength / CurrentMaxFurLength;

							if (HairLengthForceUniformity > 0)
							{
								float Relative = Vert.UVs[1].X / SplineLength;
								float Length = SplineLength * (1.0f - HairLengthForceUniformity) + CurrentMaxFurLength * HairLengthForceUniformity;
								Vert.UVs[1].X = Relative * Length;
							}
							else
							{
								float Relative = Vert.UVs[1].X / SplineLength;
								float Interpolator = -HairLengthForceUniformity;
								float Length = SplineLength * (1.0f - Interpolator) + CurrentMinFurLength * Interpolator;
								Vert.UVs[1].X = Relative * Length;
							}

							VertexBuffer.Vertices.Add(Vert);
						}
					}
					else
					{
						Vert.UVs[1].X = NonLinearFactor * InFurLength;
						Vert.UVs[1].Y = NonLinearFactor;
						Vert.UVs[2].X = LinearFactor;
						Vert.UVs[2].Y = 1.0f;
						FVector TangentZ = Vert.TangentZ;
						Vert.FurOffset = TangentZ * (NonLinearFactor * InFurLength + FMath::RandRange(-InNoiseStrength * Derivative, InNoiseStrength * Derivative));

						if (HairLengthForceUniformity > 0)
						{
							float Relative = Vert.UVs[1].X / InFurLength;
							float Length = InFurLength * (1.0f - HairLengthForceUniformity) + CurrentMaxFurLength * HairLengthForceUniformity;
							Vert.UVs[1].X = Relative * Length;
						}
						else
						{
							float Relative = Vert.UVs[1].X / InFurLength;
							float Interpolator = -HairLengthForceUniformity;
							float Length = InFurLength * (1.0f - Interpolator) + CurrentMinFurLength * Interpolator;
							Vert.UVs[1].X = Relative * Length;
						}

						VertexBuffer.Vertices.Add(Vert);
					}
				}
				if (InFurSplines)
				{
					for (uint32 t = 0; t < ModelSection.NumTriangles; ++t)
					{
						uint32 Idx0 = Indices[ModelSection.BaseIndex + t * 3];
						uint32 Idx1 = Indices[ModelSection.BaseIndex + t * 3 + 1];
						uint32 Idx2 = Indices[ModelSection.BaseIndex + t * 3 + 2];
						if (SplineMap[Idx0] >= 0 && SplineMap[Idx1] >= 0 && SplineMap[Idx2] >= 0)
						{
							Idx0 -= VertexSub[Idx0 - ModelSection.BaseVertexIndex];
							Idx1 -= VertexSub[Idx1 - ModelSection.BaseVertexIndex];
							Idx2 -= VertexSub[Idx2 - ModelSection.BaseVertexIndex];
							IndexBuffer.Indices.Add(Idx0 + IndexOffset);
							IndexBuffer.Indices.Add(Idx1 + IndexOffset);
							IndexBuffer.Indices.Add(Idx2 + IndexOffset);
						}
					}
				}
				else
				{
					for (uint32 i = 0; i < ModelSection.NumTriangles * 3; ++i)
						IndexBuffer.Indices.Add(Indices[ModelSection.BaseIndex + i] + IndexOffset);
				}
				IndexOffset = VertexBuffer.Vertices.Num();
			}
			FurSection.NumVertices = VertexBuffer.Vertices.Num() - FurSection.BaseVertexIndex;
			FurSection.NumTriangles = (IndexBuffer.Indices.Num() - FurSection.BaseIndex) / 3;
			FurSection.NumBones = ModelSection.BoneMap.Num();
		}

		// Init vertex factory
		if (VertexBuffer.Vertices.Num() == 0)
			VertexBuffer.Vertices.Add(FFurSkinVertex());
		if (IndexBuffer.Indices.Num() == 0)
			IndexBuffer.Indices.Add(0);

		// Enqueue initialization of render resource
		BeginInitResource(&VertexBuffer);
		BeginInitResource(&IndexBuffer);
	}

	~FFurData()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
	}

	UFurSplines* generateSplines(USkeletalMesh* InSkeletalMesh, int InLod, const TArray<USkeletalMesh*>& InGuideMeshes)
	{
		UFurSplines* Splines = NewObject<UFurSplines>();

		auto* SkeletalMeshResource = SkeletalMesh->GetResourceForRendering();
		check(SkeletalMeshResource);

		if (InLod >= SkeletalMeshResource->LODRenderData.Num())
			InLod = SkeletalMeshResource->LODRenderData.Num() - 1;
		const auto& LodModel = SkeletalMeshResource->LODRenderData[InLod];

		const auto& SourcePositions = LodModel.StaticVertexBuffers.PositionVertexBuffer;

		uint32 VertexCount = SourcePositions.GetNumVertices();
		int SegCount = InGuideMeshes.Num() + 1;
		Splines->Vertices.AddUninitialized(VertexCount * SegCount);
		Splines->Index.AddUninitialized(VertexCount);
		Splines->Count.AddUninitialized(VertexCount);
		for (uint32 i = 0; i < VertexCount; i++)
		{
			int Index = i * SegCount;
			Splines->Vertices[Index] = SourcePositions.VertexPosition(i);
			Splines->Index[i] = Index;
			Splines->Count[i] = SegCount;
		}

		int k = 1;
		for (USkeletalMesh* GuideMesh : InGuideMeshes)
		{
			if (GuideMesh)
			{
				auto* SkeletalMeshResource2 = GuideMesh->GetResourceForRendering();
				check(SkeletalMeshResource2);
				const auto& LodModel2 = SkeletalMeshResource2->LODRenderData[InLod];
				const auto& SourcePositions2 = LodModel2.StaticVertexBuffers.PositionVertexBuffer;
				int c = FMath::Min(SourcePositions2.GetNumVertices(), VertexCount);
				for (int i = 0; i < c; i++)
					Splines->Vertices[i * SegCount + k] = SourcePositions2.VertexPosition(i);
				for (uint32 i = c; i < VertexCount; i++)
					Splines->Vertices[i * SegCount + k] = Splines->Vertices[i * SegCount + (k - 1)];
			}
			else
			{
				for (uint32 i = 0; i < VertexCount; i++)
					Splines->Vertices[i * SegCount + k] = Splines->Vertices[i * SegCount + (k - 1)];
			}
			k++;
		}

		for (int i = 0, c = SegCount * VertexCount; i < c; i++)
			Splines->Vertices[i].Y = -Splines->Vertices[i].Y;

		return Splines;
	}
};

/** Scene proxy */
class FFurSceneProxy : public FPrimitiveSceneProxy
{
public:
	FFurSceneProxy(UGFurComponent* InComponent, const TArray<FFurData*>& InFurData, const TArray<FFurLod>& InFurLods, const TArray<UMaterialInstanceDynamic*>& InFurMaterials, bool InCastShadows, ERHIFeatureLevel::Type InFeatureLevel)
		: FPrimitiveSceneProxy(InComponent)
		, FurComponent(InComponent)
		, FurData(InFurData)
		, FurLods(InFurLods)
		, FurMaterials(InFurMaterials)
		, CastShadows(InCastShadows)
	{

		bAlwaysHasVelocity = true;

		for (auto* d : InFurData)
		{
			for (auto& s : d->Sections)
			{
				FFurVertexFactory* vf = new FFurVertexFactory(InFeatureLevel);
				vf->Init(&d->VertexBuffer, s.NumBones);
				BeginInitResource(vf);
				VertexFactories.Add(vf);
			}
		}
	}

	virtual ~FFurSceneProxy()
	{
		for (auto* VertexFactory : VertexFactories)
		{
			VertexFactory->ReleaseResource();
			delete VertexFactory;
		}
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_GetDynamicMeshElements);

		const bool Wireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f));

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		int NewLodLevel = 0x7fffffff;
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];
				static const auto* SkeletalMeshLODRadiusScale = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.SkeletalMeshLODRadiusScale"));
				float LODScale = FMath::Clamp(SkeletalMeshLODRadiusScale->GetValueOnRenderThread(), 0.25f, 1.0f);
				const float ScreenRadiusSquared = ComputeBoundsScreenRadiusSquared(FurComponent->Bounds.Origin, FurComponent->Bounds.SphereRadius, *View) * LODScale * LODScale;

				if (FMath::Square(FurComponent->MinScreenSize * 0.5f) < ScreenRadiusSquared)
				{
					bool Found = false;
					for (int32 LODLevel = FurData.Num() - 1; LODLevel > 0; LODLevel--)
					{
						// Get ScreenSize for this LOD
						float ScreenSize = FurLods[LODLevel - 1].ScreenSize;

						// If have passed this boundary, use this LOD
						if (FMath::Square(ScreenSize * 0.5f) > ScreenRadiusSquared)
						{
							NewLodLevel = FMath::Min(NewLodLevel, LODLevel);
							Found = true;
							break;
						}
					}

					if (!Found)
					{
						NewLodLevel = 0;
						break;
					}
				}
				else
				{
					NewLodLevel = FMath::Min(NewLodLevel, 0x7fffffff);
				}
			}
		}
		if (NewLodLevel != CurrentLodLevel)
		{
			CurrentLodLevel = NewLodLevel;
			SectionOffset = 0;
			for (int i = 0; i < NewLodLevel; i++)
				SectionOffset += FurData[i]->Sections.Num();
		}

		if (CurrentLodLevel < FurData.Num())
		{
			for (int sectionIdx = 0; sectionIdx < FurData[CurrentLodLevel]->Sections.Num(); sectionIdx++)
			{
				const FFurData::FSection& section = FurData[CurrentLodLevel]->Sections[sectionIdx];
				if (section.NumTriangles == 0)
					continue;
				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
				{
					if (VisibilityMap & (1 << ViewIndex))
					{
						const FSceneView* View = Views[ViewIndex];

						FMaterialRenderProxy* MaterialProxy = NULL;
						if (Wireframe)
						{
							MaterialProxy = WireframeMaterialInstance;
						}
						else
						{
							UMaterialInstanceDynamic* material = FurMaterials[sectionIdx];
							MaterialProxy = material->GetRenderProxy(IsSelected());
						}

						FMeshBatch& Mesh = Collector.AllocateMesh();
						FMeshBatchElement& BatchElement = Mesh.Elements[0];
						BatchElement.IndexBuffer = &FurData[CurrentLodLevel]->IndexBuffer;
						Mesh.bWireframe = Wireframe;
						Mesh.VertexFactory = GetVertexFactory(sectionIdx);
						Mesh.MaterialRenderProxy = MaterialProxy;
						BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
						BatchElement.FirstIndex = section.BaseIndex;
						BatchElement.NumPrimitives = section.NumTriangles;
						BatchElement.MinVertexIndex = section.BaseVertexIndex;
						BatchElement.MaxVertexIndex = section.BaseVertexIndex + section.NumVertices - 1;
						Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
						Mesh.Type = PT_TriangleList;
						Mesh.DepthPriorityGroup = SDPG_World;
						Mesh.bCanApplyViewModeOverrides = false;
						Collector.AddMesh(ViewIndex, Mesh);
					}
				}
			}
		}
	}

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
	{
/*		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_DrawDynamicElements);

		const bool bWireframe = AllowDebugViewmodes() && View->Family->EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy WireframeMaterialInstance(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
			FLinearColor(0, 0.5f, 1.f));

		FMaterialRenderProxy* MaterialProxy = NULL;
		if (bWireframe)
		{
			MaterialProxy = &WireframeMaterialInstance;
		}
		else
		{
			MaterialProxy = Material->GetRenderProxy(IsSelected());
		}


		// Draw the mesh.
		FMeshBatch Mesh;
		FMeshBatchElement& BatchElement = Mesh.Elements[0];
		BatchElement.IndexBuffer = &IndexBuffer;
		Mesh.bWireframe = bWireframe;
		Mesh.VertexFactory = &VertexFactory;
		Mesh.MaterialRenderProxy = MaterialProxy;
		BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
		BatchElement.FirstIndex = 0;
		BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
		BatchElement.MinVertexIndex = 0;
		BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
		Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
		Mesh.Type = PT_TriangleList;
		Mesh.DepthPriorityGroup = SDPG_World;
		PDI->DrawMesh(Mesh);*/
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bShadowRelevance = CastShadows;
		Result.bDynamicRelevance = true;
		//Material->GetRelevance(GetScene().GetFeatureLevel()).SetPrimitiveViewRelevance(Result);
		return Result;
	}

	virtual bool CanBeOccluded() const override { return true; }

	virtual uint32 GetMemoryFootprint(void) const { return (sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return (FPrimitiveSceneProxy::GetAllocatedSize()); }

	FFurData* GetFurData() { return FurData[FMath::Min(CurrentLodLevel, FurData.Num() - 1)]; }
	FFurVertexFactory* GetVertexFactory(int sectionIdx) const { return VertexFactories[SectionOffset + sectionIdx]; }

	int GetCurrentLodLevel() const { return CurrentLodLevel; }

	SIZE_T GetTypeHash(void) const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

private:
	UGFurComponent* FurComponent;
	TArray<FFurData*> FurData;
	TArray<FFurLod> FurLods;
	TArray<class UMaterialInstanceDynamic*> FurMaterials;
	TArray<FFurVertexFactory*> VertexFactories;
	mutable int CurrentLodLevel = 0;
	mutable int SectionOffset = 0;
	bool CastShadows;
};

//////////////////////////////////////////////////////////////////////////

static TArray< struct FFurData* > StaticFurData;
static FCriticalSection  StaticFurDataCS;

class FurDataDelayedCleanupTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FurDataDelayedCleanupTask>;

public:
	FurDataDelayedCleanupTask(const TArray<FFurData*>& InFurDataArray) : FurDataArray(InFurDataArray) {}

protected:
	const TArray<FFurData*> FurDataArray;

	void DoWork()
	{
		FPlatformProcess::Sleep(1.0f);

		StaticFurDataCS.Lock();
		for (int i = FurDataArray.Num() - 1; i != -1; i--)
		{
			FFurData* data = FurDataArray[i];
			if (--data->RefCount == 0)
			{
				StaticFurData.Remove(data);
				ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([data](FRHICommandListImmediate& RHICmdList) { delete data; });
			}
		}
		StaticFurDataCS.Unlock();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};

UGFurComponent::UGFurComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickInEditor = true;
	bAutoActivate = true;
	LayerCount = 32;
	ShellBias = 1.0f;
	FurLength = 1.0f;
	MinFurLength = 0.0f;
	ForceDistribution = 2.0f;
	Stiffness = 5.0f;
	Damping = 5.0f;
	MaxForce = 10.0f;
	MaxForceTorqueFactor = 1.0f;
	ConstantForce.Set(0, 0, -9.8f);
	ReferenceHairBias = 0.8f;
	HairLengthForceUniformity = 0.2f;
	NoiseStrength = 0.0f;
	CastShadow = false;
	PrimaryComponentTick.bCanEverTick = true;

	LastDeltaTime = 1.0f;

	SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
}

void UGFurComponent::reloadFurSplines(UFurSplines* FurSplines)
{
	StaticFurDataCS.Lock();

	for (int32 i = 0; i < StaticFurData.Num(); i++)
	{
		FFurData* Data = StaticFurData[i];

		if (Data->FurSplines == FurSplines)
		{
			USkeletalMesh* SkeletalMesh = Data->SkeletalMesh;
			int Lod = Data->Lod;
			int FurLayerCount = Data->FurLayerCount;
			float FurLength = Data->FurLength;
			float MinFurLength = Data->MinFurLength;
			float ShellBias = Data->ShellBias;
			float HairLengthForceUniformity = Data->HairLengthForceUniformity;
			float NoiseStrength = Data->NoiseStrength;

			volatile bool finished = false;
			ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([Data, &finished](FRHICommandListImmediate& RHICmdList) { Data->~FFurData(); finished = true; });
			while (!finished)
				;
			new (Data) FFurData(SkeletalMesh, Lod, FurSplines, TArray<USkeletalMesh*>(), FurLayerCount, FurLength, MinFurLength, ShellBias, HairLengthForceUniformity, NoiseStrength);
		}
	}

	StaticFurDataCS.Unlock();
}


UMaterialInterface* UGFurComponent::GetMaterial(int32 MaterialIndex) const
{
	if (MaterialIndex < OverrideMaterials.Num() && OverrideMaterials[MaterialIndex])
	{
		return OverrideMaterials[MaterialIndex];
	}
	else if (GrowMesh && MaterialIndex < GrowMesh->Materials.Num() && GrowMesh->Materials[MaterialIndex].MaterialInterface)
	{
		return GrowMesh->Materials[MaterialIndex].MaterialInterface;
	}

	return NULL;
}

int32 UGFurComponent::GetMaterialIndex(FName MaterialSlotName) const
{
	for (int32 MaterialIndex = 0; MaterialIndex < GrowMesh->Materials.Num(); ++MaterialIndex)
	{
		const FSkeletalMaterial &SkeletalMaterial = GrowMesh->Materials[MaterialIndex];
		if (SkeletalMaterial.MaterialSlotName == MaterialSlotName)
		{
			return MaterialIndex;
		}
	}
	return -1;
}

TArray<FName> UGFurComponent::GetMaterialSlotNames() const
{
	TArray<FName> MaterialNames;
	for (int32 MaterialIndex = 0; MaterialIndex < GrowMesh->Materials.Num(); ++MaterialIndex)
	{
		const FSkeletalMaterial &SkeletalMaterial = GrowMesh->Materials[MaterialIndex];
		MaterialNames.Add(SkeletalMaterial.MaterialSlotName);
	}
	return MaterialNames;
}

bool UGFurComponent::IsMaterialSlotNameValid(FName MaterialSlotName) const
{
	return GetMaterialIndex(MaterialSlotName) >= 0;
}

void UGFurComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	if (GrowMesh)
	{
		// The max number of materials used is the max of the materials on the skeletal mesh and the materials on the mesh component
		const int32 NumMaterials = FMath::Max(GrowMesh->Materials.Num(), OverrideMaterials.Num());
		for (int32 MatIdx = 0; MatIdx < NumMaterials; ++MatIdx)
		{
			// GetMaterial will determine the correct material to use for this index.  
			UMaterialInterface* MaterialInterface = GetMaterial(MatIdx);
			OutMaterials.Add(MaterialInterface);
		}

		for (UMaterialInstanceDynamic* material : FurMaterials)
			OutMaterials.Add(material);
	}
}

bool UGFurComponent::GetMaterialStreamingData(int32 MaterialIndex, FPrimitiveMaterialInfo& MaterialData) const
{
	if (GrowMesh)
	{
		MaterialData.Material = GetMaterial(MaterialIndex);
		MaterialData.UVChannelData = GrowMesh->GetUVChannelData(MaterialIndex);
		MaterialData.PackedRelativeBox = PackedRelativeBox_Identity;
	}
	return MaterialData.IsValid();
}

void UGFurComponent::GetStreamingTextureInfo(FStreamingTextureLevelContext& LevelContext, TArray<FStreamingTexturePrimitiveInfo>& OutStreamingTextures) const
{
	GetStreamingTextureInfoInner(LevelContext, nullptr, GetComponentTransform().GetMaximumAxisScale(), OutStreamingTextures);
}

int32 UGFurComponent::GetNumMaterials() const
{
	if (GrowMesh)
	{
		return GrowMesh->Materials.Num();
	}

	return 0;
}


FPrimitiveSceneProxy* UGFurComponent::CreateSceneProxy()
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		TArray<USceneComponent*> parents;
		GetParentComponents(parents);
		for (USceneComponent* Comp : parents)
		{
			if (Comp->IsA(USkeletalMeshComponent::StaticClass()))
			{
				MasterPoseComponent = (USkeletalMeshComponent*)Comp;
				break;
			}
		}

		UpdateMasterBoneMap();
		if (GrowMesh && GrowMesh->GetResourceForRendering())
		{
			TArray<FFurData*> Array;
			Array.Add(GetOrCreateFurData(LayerCount, 0));
			for (FFurLod& lod : LODs)
				Array.Add(GetOrCreateFurData(lod.LayerCount, FMath::Min(GrowMesh->GetResourceForRendering()->LODRenderData.Num() - 1, lod.Lod)));

			FurData = Array;
			return new FFurSceneProxy(this, FurData, LODs, FurMaterials, CastShadow, GetWorld()->FeatureLevel);
		}
	}
	return nullptr;
}


void UGFurComponent::CreateRenderState_Concurrent()
{
	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		for (int i = 0, c = GetNumMaterials(); i < c; i++)
		{
			UMaterialInterface* tmp_material = GetMaterial(i);
			if (tmp_material == NULL)
			{
				tmp_material = UMaterial::GetDefaultMaterial(MD_Surface);
			}
			UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(tmp_material, this);
			Material->AddToRoot();
			Material->SetScalarParameterValue(FName(TEXT("FurLength")), FMath::Max(FurLength, 0.001f));
			FurMaterials.Add(Material);
		}
	}

	Super::CreateRenderState_Concurrent();

	updateFur();
}


void UGFurComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();

	updateFur();
}


void UGFurComponent::DestroyRenderState_Concurrent()
{
	Super::DestroyRenderState_Concurrent();

	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		for (auto& mat : FurMaterials)
			mat->RemoveFromRoot();
		FurMaterials.Reset();

		(new FAutoDeleteAsyncTask<FurDataDelayedCleanupTask>(FurData))->StartBackgroundTask();
		FurData.Empty();
	}
}


void UGFurComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	LastDeltaTime = DeltaTime;

	MarkRenderDynamicDataDirty();
}


FBoxSphereBounds UGFurComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	if (MasterPoseComponent.IsValid())
	{
		FBoxSphereBounds MasterBounds = MasterPoseComponent->CalcBounds(LocalToWorld);
		MasterBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
		return MasterBounds;
	}

	FBoxSphereBounds DummyBounds = GrowMesh ? GrowMesh->GetBounds() : FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0);
	DummyBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
	return DummyBounds.TransformBy(LocalToWorld);
}


UBodySetup* UGFurComponent::GetBodySetup()
{
	return nullptr;
}


void UGFurComponent::updateFur()
{
	if (!SceneProxy)
		return;
	FFurSceneProxy* Scene = (FFurSceneProxy*)SceneProxy;

	const USkeletalMesh* const ThisMesh = GrowMesh;
	const USkinnedMeshComponent* const MasterComp = MasterPoseComponent.Get();
	const USkeletalMesh* const MasterCompMesh = MasterComp ? MasterComp->SkeletalMesh : nullptr;
	const auto& LOD = GrowMesh->GetResourceForRendering()->LODRenderData[Scene->GetCurrentLodLevel()];

	FMatrix TempMatrices[256];
	bool ValidTempMatrices[256];
	memset(ValidTempMatrices, 0, sizeof(ValidTempMatrices));
	check(ThisMesh->RefBasesInvMatrix.Num() != 0);
	bool OldPositionValid = true;
	if (ReferenceToLocal.Num() != ThisMesh->RefBasesInvMatrix.Num())
	{
		Transformations.Reset();
		Transformations.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		ReferenceToLocal.Reset();
		ReferenceToLocal.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		LinearVelocities.Reset();
		LinearVelocities.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		AngularVelocities.Reset();
		AngularVelocities.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		LinearOffsets.Reset();
		LinearOffsets.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		AngularOffsets.Reset();
		AngularOffsets.AddUninitialized(ThisMesh->RefBasesInvMatrix.Num());
		OldPositionValid = false;
	}

	FMatrix toWorld = GetComponentTransform().ToMatrixNoScale();

	const bool bIsMasterCompValid = MasterComp && MasterBoneMap.Num() == ThisMesh->RefSkeleton.GetNum();

	const TArray<FBoneIndexType>* RequiredBoneSets[3] = { &LOD.ActiveBoneIndices, 0/*ExtraRequiredBoneIndices*/, NULL };

	// Handle case of using ParentAnimComponent for SpaceBases.
	for (int32 RequiredBoneSetIndex = 0; RequiredBoneSets[RequiredBoneSetIndex] != NULL; RequiredBoneSetIndex++)
	{
		const TArray<FBoneIndexType>& RequiredBoneIndices = *RequiredBoneSets[RequiredBoneSetIndex];

		// Get the index of the bone in this skeleton, and loop up in table to find index in parent component mesh.
		for (int32 BoneIndex = 0; BoneIndex < RequiredBoneIndices.Num(); BoneIndex++)
		{
			const int32 ThisBoneIndex = RequiredBoneIndices[BoneIndex];

			if (ThisMesh->RefBasesInvMatrix.IsValidIndex(ThisBoneIndex))
			{
				// On the off chance the parent matrix isn't valid, revert to identity.
				TempMatrices[ThisBoneIndex] = FMatrix::Identity;

				if (bIsMasterCompValid)
				{
					// If valid, use matrix from parent component.
					const int32 MasterBoneIndex = MasterBoneMap[ThisBoneIndex];
					if (MasterComp->GetComponentSpaceTransforms().IsValidIndex(MasterBoneIndex))
					{
						const int32 ParentIndex = ThisMesh->RefSkeleton.GetParentIndex(ThisBoneIndex);
						bool bNeedToHideBone = MasterComp->BoneVisibilityStates[MasterBoneIndex] != BVS_Visible;
						if (bNeedToHideBone && ParentIndex != INDEX_NONE)
						{
							TempMatrices[ThisBoneIndex] = TempMatrices[ParentIndex].ApplyScale(0.f);
						}
						else
						{
							checkSlow(MasterComp->GetComponentSpaceTransforms()[MasterBoneIndex].IsRotationNormalized());
							TempMatrices[ThisBoneIndex] = MasterComp->GetComponentSpaceTransforms()[MasterBoneIndex].ToMatrixWithScale();
						}
						ValidTempMatrices[ThisBoneIndex] = true;
					}
				}
				else
				{
					TempMatrices[ThisBoneIndex] = ThisMesh->RefBasesInvMatrix[ThisBoneIndex].Inverse();
					ValidTempMatrices[ThisBoneIndex] = true;
				}
			}
			// removed else statement to set ReferenceToLocal[ThisBoneIndex] = FTransform::Identity;
			// since it failed in ( ThisMesh->RefBasesInvMatrix.IsValidIndex(ThisBoneIndex) ), ReferenceToLocal is not valid either
			// because of the initialization code line above to match both array count
			// if(ReferenceToLocal.Num() != ThisMesh->RefBasesInvMatrix.Num())
		}
	}

	if (OldPositionValid)
	{
		float DeltaTime = fminf(LastDeltaTime, 1.0f);
		float ReferenceFurLength = FMath::Max(0.00001f, Scene->GetFurData()->CurrentMaxFurLength * ReferenceHairBias + Scene->GetFurData()->CurrentMinFurLength * (1.0f - ReferenceHairBias));
//		float ForceFactor = 1.0f / (powf(ReferenceFurLength, FurForcePower) * fmaxf(FurStiffness, 0.000001f));
		float ForceFactor = 1.0f / powf(ReferenceFurLength, ForceDistribution);
		float DampingClamped = fmaxf(Damping, 0.000001f);
		float DampingFactor = powf(1.0f - (DampingClamped / (DampingClamped + 1.0f)), DeltaTime);
		float MaxForceFinal = (MaxForce * ReferenceFurLength) / powf(ReferenceFurLength, ForceDistribution);
		float MaxTorque = MaxForceTorqueFactor * MaxForceFinal / Scene->GetFurData()->MaxVertexBoneDistance;
//		FVector FurForceFinal = FurForce * (fmaxf(FurWeight, 0.000001f) * ForceFactor);//TODO
		FVector FurForceFinal = ConstantForce * ReferenceFurLength * ForceFactor / Stiffness;//TODO

		float x = DeltaTime * Stiffness;

		for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ++ThisBoneIndex)
		{
			FMatrix mat;
			if (ValidTempMatrices[ThisBoneIndex])
			{
				ReferenceToLocal[ThisBoneIndex] = ThisMesh->RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
				mat = TempMatrices[ThisBoneIndex] * toWorld;
			}
			else
			{
				ReferenceToLocal[ThisBoneIndex] = FMatrix::Identity;
				mat = FMatrix::Identity;
			}

			FVector d = (mat.GetOrigin() - Transformations[ThisBoneIndex].GetOrigin());
			d *= ForceFactor;
			LinearOffsets[ThisBoneIndex] -= d;

//			FVector force = -FurOffset[ThisBoneIndex] - LastLinearVelocities[ThisBoneIndex] * DragFactor + FurForceFinal;
//			FVector delta_velocity = force * deltaTime * RcpWeight;
//			FurOffset[ThisBoneIndex] += LastLinearVelocities[ThisBoneIndex] * deltaTime + delta_velocity * 0.5f * deltaTime;
//			LastLinearVelocities[ThisBoneIndex] += delta_velocity;
//			LastLinearVelocities[ThisBoneIndex] = LastLinearVelocities[ThisBoneIndex] * DampingFactor;
			FVector force;
			FVector newOffset = (LinearVelocities[ThisBoneIndex] * sinf(x) + (LinearOffsets[ThisBoneIndex] - FurForceFinal) * cosf(x)) * DampingFactor + FurForceFinal;
			FVector newVelocity = (LinearVelocities[ThisBoneIndex] * cosf(x) - (LinearOffsets[ThisBoneIndex] - FurForceFinal) * sinf(x)) * DampingFactor;
			check(newOffset.X == newOffset.X && newOffset.Y == newOffset.Y && newOffset.Z == newOffset.Z);
			check(newVelocity.X == newVelocity.X && newVelocity.Y == newVelocity.Y && newVelocity.Z == newVelocity.Z);
			LinearOffsets[ThisBoneIndex] = newOffset;
			LinearVelocities[ThisBoneIndex] = newVelocity;
			if (LinearOffsets[ThisBoneIndex].Size() > MaxForceFinal)
			{
				LinearOffsets[ThisBoneIndex] *= MaxForceFinal / LinearOffsets[ThisBoneIndex].Size();
				float k = FVector::DotProduct(LinearOffsets[ThisBoneIndex], LinearVelocities[ThisBoneIndex]) / FVector::DotProduct(LinearOffsets[ThisBoneIndex], LinearOffsets[ThisBoneIndex]);
				if (k > 0.0f)
					LinearVelocities[ThisBoneIndex] -= LinearOffsets[ThisBoneIndex] * k;
			}

			FQuat rdiff = mat.ToQuat() * Transformations[ThisBoneIndex].ToQuat().Inverse();
			float angle;
			rdiff.ToAxisAndAngle(d, angle);
			if (angle > PI)
				angle -= 2 * PI;
			d *= -angle * ForceFactor;
			AngularOffsets[ThisBoneIndex] -= d;
/*			force = -LastTorques[ThisBoneIndex] - LastAngularVelocities[ThisBoneIndex] * DragFactor;
			FVector delta_angular_velocity = force * deltaTime * RcpWeight;
			LastTorques[ThisBoneIndex] += LastAngularVelocities[ThisBoneIndex] * deltaTime + delta_angular_velocity * 0.5f * deltaTime;
			if (LastTorques[ThisBoneIndex].Size() > MaxTorque)
				LastTorques[ThisBoneIndex] *= MaxTorque / LastTorques[ThisBoneIndex].Size();
			LastAngularVelocities[ThisBoneIndex] += delta_angular_velocity;
			LastAngularVelocities[ThisBoneIndex] = LastAngularVelocities[ThisBoneIndex] * DampingFactor;*/
			newOffset = (AngularVelocities[ThisBoneIndex] * sinf(x) + AngularOffsets[ThisBoneIndex] * cosf(x)) * DampingFactor;
			newVelocity = (AngularVelocities[ThisBoneIndex] * cosf(x) - AngularOffsets[ThisBoneIndex] * sinf(x)) * DampingFactor;
			AngularOffsets[ThisBoneIndex] = newOffset;
			AngularVelocities[ThisBoneIndex] = newVelocity;
			if (AngularOffsets[ThisBoneIndex].Size() > MaxTorque)
				AngularOffsets[ThisBoneIndex] *= MaxTorque / AngularOffsets[ThisBoneIndex].Size();

			Transformations[ThisBoneIndex] = mat;
		}
	}
	else
	{
		for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ++ThisBoneIndex)
		{
			if (ValidTempMatrices[ThisBoneIndex])
			{
				ReferenceToLocal[ThisBoneIndex] = ThisMesh->RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
				Transformations[ThisBoneIndex] = TempMatrices[ThisBoneIndex] * toWorld;
			}
			else
			{
				ReferenceToLocal[ThisBoneIndex] = FMatrix::Identity;
				Transformations[ThisBoneIndex] = FMatrix::Identity;
			}

			LinearOffsets[ThisBoneIndex].Set(0.0f, 0.0f, 0.0f);
			AngularOffsets[ThisBoneIndex].Set(0.0f, 0.0f, 0.0f);

			LinearVelocities[ThisBoneIndex].Set(0.0f, 0.0f, 0.0f);
			AngularVelocities[ThisBoneIndex].Set(0.0f, 0.0f, 0.0f);
		}
	}

	// We prepare the next frame but still have the value from the last one
	uint32 FrameNumberToPrepare = GFrameNumber + 1;

	// queue a call to update this data
	ENQUEUE_RENDER_COMMAND(SkelMeshObjectUpdateDataCommand)(
		[this, FrameNumberToPrepare](FRHICommandListImmediate& RHICmdList)
	{
		UpdateFur_RenderThread(RHICmdList, FrameNumberToPrepare);
	}
	);
}

void UGFurComponent::UpdateFur_RenderThread(FRHICommandListImmediate& RHICmdList, uint32 FrameNumberToPrepare)
{
	FFurSceneProxy* FurProxy = (FFurSceneProxy*)SceneProxy;

	if (FurProxy)
	{
		const auto& LOD = GrowMesh->GetResourceForRendering()->LODRenderData[FurProxy->GetCurrentLodLevel()];
		const auto& Sections = LOD.RenderSections;

		ERHIFeatureLevel::Type SceneFeatureLevel = GetWorld()->FeatureLevel;
		for (int32 SectionIdx = 0; SectionIdx < Sections.Num(); SectionIdx++)
		{
			FFurVertexFactory::FShaderDataType& ShaderData = FurProxy->GetVertexFactory(SectionIdx)->ShaderData;
			ShaderData.FurOffsetPower = ForceDistribution;
			ShaderData.UpdateBoneData(ReferenceToLocal, LinearOffsets, AngularOffsets, Transformations, Sections[SectionIdx].BoneMap, FrameNumberToPrepare, SceneFeatureLevel);
		}
	}
}

void UGFurComponent::UpdateMasterBoneMap()
{
	MasterBoneMap.Empty();

	if (GrowMesh && MasterPoseComponent.IsValid() && MasterPoseComponent->SkeletalMesh)
	{
		USkeletalMesh* ParentMesh = MasterPoseComponent->SkeletalMesh;

		MasterBoneMap.Empty(GrowMesh->RefSkeleton.GetNum());
		MasterBoneMap.AddUninitialized(GrowMesh->RefSkeleton.GetNum());
		if (GrowMesh == ParentMesh)
		{
			// if the meshes are the same, the indices must match exactly so we don't need to look them up
			for (int32 i = 0; i < MasterBoneMap.Num(); i++)
			{
				MasterBoneMap[i] = i;
			}
		}
		else
		{
			for (int32 i = 0; i<MasterBoneMap.Num(); i++)
			{
				FName BoneName = GrowMesh->RefSkeleton.GetBoneName(i);
				MasterBoneMap[i] = ParentMesh->RefSkeleton.FindBoneIndex(BoneName);
			}
		}
	}
}

FFurData* UGFurComponent::GetOrCreateFurData(int InFurLayerCount, int InLod)
{
	if (InFurLayerCount < 1)
		InFurLayerCount = 1;
	if (InFurLayerCount > 128)
		InFurLayerCount = 128;

	float FurLengthClamped = FMath::Max(FurLength, 0.001f);

	StaticFurDataCS.Lock();

	FFurData* Data = nullptr;
	for (int32 i = 0; i < StaticFurData.Num(); i++)
	{
		FFurData* d = StaticFurData[i];
		if (d->SkeletalMesh == GrowMesh && d->Lod == InLod && d->FurLayerCount == InFurLayerCount && d->FurSplines == FurSplines
			&& d->GuideMeshes == GuideMeshes
			&& d->FurLength == FurLengthClamped && d->MinFurLength == MinFurLength
			&& d->ShellBias == ShellBias && d->HairLengthForceUniformity == HairLengthForceUniformity && d->NoiseStrength == NoiseStrength)
		{
			Data = d;
			break;
		}
	}
	if (Data)
	{
		Data->RefCount++;
	}
	else
	{
		Data = new FFurData(GrowMesh, InLod, FurSplines, GuideMeshes, InFurLayerCount, FurLengthClamped, MinFurLength, ShellBias, HairLengthForceUniformity, NoiseStrength);
		Data->RefCount = 1;
		StaticFurData.Add(Data);
	}

	StaticFurDataCS.Unlock();

	return Data;
}
