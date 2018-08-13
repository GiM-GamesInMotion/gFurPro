// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurStaticData.h"
#include "ShaderParameterUtils.h"
#include "FurComponent.h"

static TArray< struct FFurStaticData* > StaticFurStaticData;
static FCriticalSection  StaticFurStaticDataCS;

/** Soft Static Vertex */
struct FStaticVertex
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
};

/** Fur Static Vertex */
struct FFurStaticVertex : FStaticVertex
{
	FVector FurOffset;
};

/** Vertex Buffer */
class FFurStaticVertexBuffer : public FVertexBuffer
{
public:
	TArray<FFurStaticVertex> Vertices;

	virtual void InitRHI() override;
};

void FFurStaticVertexBuffer::InitRHI()
{
	FRHIResourceCreateInfo CreateInfo;
	VertexBufferRHI = RHICreateVertexBuffer(Vertices.Num() * sizeof(FFurStaticVertex), BUF_Static, CreateInfo);
	// Copy the vertex data into the vertex buffer.
	void* VertexBufferData = RHILockVertexBuffer(VertexBufferRHI, 0, Vertices.Num() * sizeof(FFurStaticVertex), RLM_WriteOnly);
	FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FFurStaticVertex));
	RHIUnlockVertexBuffer(VertexBufferRHI);
}

/** Vertex Factory Shader Parameters */
class FFurStaticVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
public:
	void Bind(const FShaderParameterMap& ParameterMap) override
	{
		MeshOriginParameter.Bind(ParameterMap, TEXT("MeshOrigin"));
		MeshExtensionParameter.Bind(ParameterMap, TEXT("MeshExtension"));
		FurOffsetPowerParameter.Bind(ParameterMap, TEXT("FurOffsetPower"));
		FurLinearOffsetParameter.Bind(ParameterMap, TEXT("FurLinearOffset"));
		FurPositionParameter.Bind(ParameterMap, TEXT("FurPosition"));
		FurAngularOffsetParameter.Bind(ParameterMap, TEXT("FurAngularOffset"));
		PreviousFurLinearOffsetParameter.Bind(ParameterMap, TEXT("PreviousFurLinearOffset"));
		PreviousFurPositionParameter.Bind(ParameterMap, TEXT("PreviousFurPosition"));
		PreviousFurAngularOffsetParameter.Bind(ParameterMap, TEXT("PreviousFurAngularOffset"));
	}


	void Serialize(FArchive& Ar) override
	{
		Ar << MeshOriginParameter;
		Ar << MeshExtensionParameter;
		Ar << FurOffsetPowerParameter;
		Ar << FurLinearOffsetParameter;
		Ar << FurPositionParameter;
		Ar << FurAngularOffsetParameter;
		Ar << PreviousFurLinearOffsetParameter;
		Ar << PreviousFurPositionParameter;
		Ar << PreviousFurAngularOffsetParameter;
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
	FShaderParameter FurLinearOffsetParameter;
	FShaderParameter FurPositionParameter;
	FShaderParameter FurAngularOffsetParameter;
	FShaderParameter PreviousFurLinearOffsetParameter;
	FShaderParameter PreviousFurPositionParameter;
	FShaderParameter PreviousFurAngularOffsetParameter;
};

/** Vertex Factory */
class FFurStaticVertexFactory : public FFurVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE(FFurStaticVertexFactory);

public:
	struct FShaderDataType
	{
		/** Mesh origin and Mesh Extension for Mesh compressions **/
		/** This value will be (0, 0, 0), (1, 1, 1) relatively for non compressed meshes **/
		FVector MeshOrigin;
		FVector MeshExtension;
		float FurOffsetPower;
		FVector FurLinearOffset;
		FVector FurPosition;
		FVector FurAngularOffset;
		FVector PreviousFurLinearOffset;
		FVector PreviousFurPosition;
		FVector PreviousFurAngularOffset;

		FShaderDataType()
			: PreviousFrameNumber(0)
			, CurrentFrameNumber(0)
			, MeshOrigin(0, 0, 0)
			, MeshExtension(1, 1, 1)
			, FurOffsetPower(2.0f)
			, FurLinearOffset(0, 0, 0)
			, FurPosition(0, 0, 0)
			, FurAngularOffset(0, 0, 0)
			, PreviousFurLinearOffset(0, 0, 0)
			, PreviousFurPosition(0, 0, 0)
			, PreviousFurAngularOffset(0, 0, 0)
		{
		}

		void GoToNextFrame(uint32 FrameNumber);

		bool IsPreviousDataValid()
		{
			return CurrentFrameNumber == PreviousFrameNumber + 1;
		}

	private:
		// from GFrameNumber, to detect pause and old data when an object was not rendered for some time
		uint32 PreviousFrameNumber;
		uint32 CurrentFrameNumber;
	};

	FFurStaticVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurVertexFactory(InFeatureLevel)
	{
	}

	struct FDataType
	{
		FVertexStreamComponent PositionComponent;
		FVertexStreamComponent TangentBasisComponents[2];
		TArray<FVertexStreamComponent, TFixedAllocator<MAX_TEXCOORDS>> TextureCoordinates;
		FVertexStreamComponent ColorComponent;
		FVertexStreamComponent FurOffset;
	};

	void Init(const FFurStaticVertexBuffer* VertexBuffer)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(InitProceduralMeshVertexFactory,
			FFurStaticVertexFactory*,
			VertexFactory,
			this,
			const FFurStaticVertexBuffer*,
			VertexBuffer,
			VertexBuffer,
			{
				// Initialize the vertex factory's stream components.
				FDataType NewData;
		NewData.PositionComponent =
			STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurStaticVertex, Position, VET_Float3);
		int c = sizeof(FFurStaticVertex::UVs) / sizeof(FFurStaticVertex::UVs[0]);
		for (int i = 0; i < c; ++i)
		{
			NewData.TextureCoordinates.Add(FVertexStreamComponent(
				VertexBuffer, STRUCT_OFFSET(FFurStaticVertex, UVs[i]), sizeof(FFurStaticVertex), VET_Float2));
		}
		NewData.TangentBasisComponents[0] =
			STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurStaticVertex, TangentX, VET_PackedNormal);
		NewData.TangentBasisComponents[1] =
			STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurStaticVertex, TangentZ, VET_PackedNormal);
		NewData.ColorComponent =
			STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurStaticVertex, Color, VET_Color);
		NewData.FurOffset =
			STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FFurStaticVertex, FurOffset, VET_Float3);

		VertexFactory->SetData(NewData);
			});
	}

	static FVertexFactoryShaderParameters* ConstructShaderParameters(EShaderFrequency ShaderFrequency)
	{
		return (ShaderFrequency == SF_Vertex) ? new FFurStaticVertexFactoryShaderParameters() : NULL;
	}


	static bool ShouldCache(EShaderPlatform Platform,
		const class FMaterial* Material,
		const class FShaderType* ShaderType)
	{
		return true;
	}

	static bool ShouldCompilePermutation(EShaderPlatform Platform, const class FMaterial* Material, const FShaderType* ShaderType)
	{
		return true;
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
			const uint8 BaseTexCoordAttribute = 4;
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
			OutElements.Add(AccessStreamComponent(InData.ColorComponent, 3));
		}
		else
		{
			FVertexStreamComponent NullColorComponent(&GNullColorVertexBuffer, 0, 0, VET_Color);
			OutElements.Add(AccessStreamComponent(NullColorComponent, 3));
		}
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

	virtual void UpdateStaticShaderData(float InFurOffsetPower, const FVector& InLinearOffset, const FVector& InAngularOffset,
		const FVector& InPosition, uint32 InFrameNumber, ERHIFeatureLevel::Type InFeatureLevel) override
	{
		ShaderData.GoToNextFrame(InFrameNumber);

		ShaderData.FurOffsetPower = InFurOffsetPower;

		ShaderData.PreviousFurLinearOffset = ShaderData.FurLinearOffset;
		ShaderData.PreviousFurAngularOffset = ShaderData.FurAngularOffset;
		ShaderData.PreviousFurPosition = ShaderData.FurPosition;
		
		ShaderData.FurLinearOffset = InLinearOffset;
		ShaderData.FurAngularOffset = InAngularOffset;
		ShaderData.FurPosition = InPosition;
	}

	FDataType Data;
	FShaderDataType ShaderData;
};

IMPLEMENT_VERTEX_FACTORY_TYPE(FFurStaticVertexFactory, "/Plugin/gFur/Private/GFurStaticFactory.ush", true, false, true, true, false);

void FFurStaticVertexFactory::FShaderDataType::GoToNextFrame(uint32 FrameNumber)
{
	PreviousFrameNumber = CurrentFrameNumber;
	CurrentFrameNumber = FrameNumber;
}

void FFurStaticVertexFactoryShaderParameters::SetMesh(FRHICommandList& RHICmdList, FShader* Shader, const FVertexFactory* VertexFactory, const FSceneView& View, const FMeshBatchElement& BatchElement, uint32 DataFlags) const
{
	FRHIVertexShader* ShaderRHI = Shader->GetVertexShader();

	if (ShaderRHI)
	{
		FFurStaticVertexFactory::FShaderDataType& ShaderData = ((FFurStaticVertexFactory*)VertexFactory)->ShaderData;

		SetShaderValue(RHICmdList, ShaderRHI, MeshOriginParameter, ShaderData.MeshOrigin);
		SetShaderValue(RHICmdList, ShaderRHI, MeshExtensionParameter, ShaderData.MeshExtension);
		SetShaderValue(RHICmdList, ShaderRHI, FurOffsetPowerParameter, ShaderData.FurOffsetPower);
		SetShaderValue(RHICmdList, ShaderRHI, FurLinearOffsetParameter, ShaderData.FurLinearOffset);
		SetShaderValue(RHICmdList, ShaderRHI, FurPositionParameter, ShaderData.FurPosition);
		SetShaderValue(RHICmdList, ShaderRHI, FurAngularOffsetParameter, ShaderData.FurAngularOffset);

		if (ShaderData.IsPreviousDataValid())
		{
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurLinearOffsetParameter, ShaderData.PreviousFurLinearOffset);
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurPositionParameter, ShaderData.PreviousFurPosition);
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurAngularOffsetParameter, ShaderData.PreviousFurAngularOffset);
		}
		else
		{
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurLinearOffsetParameter, ShaderData.FurLinearOffset);
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurPositionParameter, ShaderData.FurPosition);
			SetShaderValue(RHICmdList, ShaderRHI, PreviousFurAngularOffsetParameter, ShaderData.FurAngularOffset);
		}
	}
}

/** Fur Skin Data */
FFurStaticData::~FFurStaticData()
{
	VertexBuffer->ReleaseResource();
	delete VertexBuffer;
	IndexBuffer.ReleaseResource();
}

FFurStaticData::FFurStaticData(UStaticMesh* InStaticMesh, int InLod, UFurSplines* InFurSplines, const TArray<UStaticMesh*>& InGuideMeshes, int InFurLayerCount,
	float InFurLength, float InMinFurLength, float InShellBias, float InHairLengthForceUniformity, float InNoiseStrength)
{
	StaticMesh = InStaticMesh;
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

	VertexBuffer = new FFurStaticVertexBuffer();

	if (InFurSplines == nullptr && InGuideMeshes.Num() > 0)
		InFurSplines = GenerateSplines(InStaticMesh, InLod, InGuideMeshes);

	auto* SkeletalMeshResource = StaticMesh->RenderData.Get();
	check(SkeletalMeshResource);

	TArray<uint32> Indices;
	TArray<int32> SplineMap;
	TArray<uint32> VertexSub;

	const auto& LodModel = SkeletalMeshResource->LODResources[InLod];
	const auto& SourcePositions = LodModel.VertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodModel.VertexBuffers.StaticMeshVertexBuffer;
	const auto& SourceColors = LodModel.VertexBuffers.ColorVertexBuffer;
	LodModel.IndexBuffer.GetCopy(Indices);

	check(SourcePositions.GetNumVertices() == SourceVertices.GetNumVertices());

	uint32 NumTexCoords = SourceVertices.GetNumTexCoords();
	bool hasVertexColor = SourceColors.GetNumVertices() > 0;
	check(!hasVertexColor || SourcePositions.GetNumVertices() == SourceColors.GetNumVertices());

	float MaxDistSq = 0;
	for (uint32 i = 0; i < SourcePositions.GetNumVertices(); i++)
	{
		const auto& Position = SourcePositions.VertexPosition(i);
		float d = Position.SizeSquared();
		if (d > MaxDistSq)
			MaxDistSq = d;
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

	const uint32 NumVertices = SourcePositions.GetNumVertices();
	if (InFurSplines)
	{
		VertexSub.Reset(NumVertices);
		VertexSub.AddUninitialized(NumVertices);
		uint32* Vert = &VertexSub[0];
		uint32 Count = 0;
		for (uint32 i = 0; i < NumVertices; ++i)
		{
			int Index = SplineMap[i];
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
		for (uint32 i = 0; i < NumVertices; ++i)
		{
			FFurStaticVertex Vert;
			uint32 SourceVertexIndex = i;
			Vert.Position = SourcePositions.VertexPosition(SourceVertexIndex);
			Vert.TangentX = SourceVertices.VertexTangentX(SourceVertexIndex);
			Vert.TangentY = SourceVertices.VertexTangentY(SourceVertexIndex);
			Vert.TangentZ = SourceVertices.VertexTangentZ(SourceVertexIndex);
			for (uint32 tc = 0; tc < NumTexCoords; tc++)
				Vert.UVs[tc] = SourceVertices.GetVertexUV(SourceVertexIndex, tc);
			Vert.Color = hasVertexColor ? SourceColors.VertexColor(SourceVertexIndex) : FColor(255, 255, 255, 255);
			uint32 ib = 0;

			if (InFurSplines)
			{
				int Index = SplineMap[i];
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
					FVector TangentZ = Vert.TangentZ.ToFVector();
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

					VertexBuffer->Vertices.Add(Vert);
				}
			}
			else
			{
				Vert.UVs[1].X = NonLinearFactor * InFurLength;
				Vert.UVs[1].Y = NonLinearFactor;
				Vert.UVs[2].X = LinearFactor;
				Vert.UVs[2].Y = 1.0f;
				FVector TangentZ = Vert.TangentZ.ToFVector();
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

				VertexBuffer->Vertices.Add(Vert);
			}
		}
	}


	for (int SectionIndex = 0; SectionIndex < LodModel.Sections.Num(); SectionIndex++)
	{
		const auto& ModelSection = LodModel.Sections[SectionIndex];
		FSection& FurSection = Sections[Sections.AddUninitialized()];
		new (&FurSection) FSection();
		FurSection.MaterialIndex = ModelSection.MaterialIndex;
		FurSection.MinVertexIndex = 0;
		FurSection.MaxVertexIndex = VertexBuffer->Vertices.Num() - 1;
		FurSection.BaseIndex = IndexBuffer.Indices.Num();

		for (int Layer = FurLayerCount; Layer > 0; --Layer)
		{
			int IndexOffset = (Layer - 1) * NumVertices;
			check(IndexOffset >= 0);
			if (InFurSplines)
			{
				for (uint32 t = 0; t < ModelSection.NumTriangles; ++t)
				{
					uint32 Idx0 = Indices[ModelSection.FirstIndex + t * 3];
					uint32 Idx1 = Indices[ModelSection.FirstIndex + t * 3 + 1];
					uint32 Idx2 = Indices[ModelSection.FirstIndex + t * 3 + 2];
					if (SplineMap[Idx0] >= 0 && SplineMap[Idx1] >= 0 && SplineMap[Idx2] >= 0)
					{
						Idx0 -= VertexSub[Idx0];
						Idx1 -= VertexSub[Idx1];
						Idx2 -= VertexSub[Idx2];
						IndexBuffer.Indices.Add(Idx0 + IndexOffset);
						IndexBuffer.Indices.Add(Idx1 + IndexOffset);
						IndexBuffer.Indices.Add(Idx2 + IndexOffset);
					}
				}
			}
			else
			{
				for (uint32 i = 0; i < ModelSection.NumTriangles * 3; ++i)
					IndexBuffer.Indices.Add(Indices[ModelSection.FirstIndex + i] + IndexOffset);
			}
		}
		FurSection.NumTriangles = (IndexBuffer.Indices.Num() - FurSection.BaseIndex) / 3;
		FurSection.NumBones = 0;
	}

	// Init vertex factory
	if (VertexBuffer->Vertices.Num() == 0)
		VertexBuffer->Vertices.Add(FFurStaticVertex());
	if (IndexBuffer.Indices.Num() == 0)
		IndexBuffer.Indices.Add(0);

	// Enqueue initialization of render resource
	BeginInitResource(VertexBuffer);
	BeginInitResource(&IndexBuffer);
}

UFurSplines* FFurStaticData::GenerateSplines(UStaticMesh* InStaticMesh, int InLod, const TArray<UStaticMesh*>& InGuideMeshes)
{
	UFurSplines* Splines = NewObject<UFurSplines>();

	auto* StaticMeshResource = InStaticMesh->RenderData.Get();
	check(StaticMeshResource);

	if (InLod >= StaticMeshResource->LODResources.Num())
		InLod = StaticMeshResource->LODResources.Num() - 1;
	const auto& LodModel = StaticMeshResource->LODResources[InLod];

	const auto& SourcePositions = LodModel.VertexBuffers.PositionVertexBuffer;

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
	for (UStaticMesh* GuideMesh : InGuideMeshes)
	{
		if (GuideMesh)
		{
			auto* StaticMeshResource2 = GuideMesh->RenderData.Get();
			check(StaticMeshResource2);
			const auto& LodModel2 = StaticMeshResource2->LODResources[InLod];
			const auto& SourcePositions2 = LodModel2.VertexBuffers.PositionVertexBuffer;
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

void FFurStaticData::CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, ERHIFeatureLevel::Type InFeatureLevel)
{
	for (auto& s : Sections)
	{
		FFurStaticVertexFactory* vf = new FFurStaticVertexFactory(InFeatureLevel);
		vf->Init(VertexBuffer);
		BeginInitResource(vf);
		VertexFactories.Add(vf);
	}
}

void FFurStaticData::ReloadFurSplines(UFurSplines* FurSplines)
{
	StaticFurStaticDataCS.Lock();

	for (int32 i = 0; i < StaticFurStaticData.Num(); i++)
	{
		FFurStaticData* Data = StaticFurStaticData[i];

		if (Data->FurSplines == FurSplines)
		{
			UStaticMesh* StaticMesh = Data->StaticMesh;
			int Lod = Data->Lod;
			int FurLayerCount = Data->FurLayerCount;
			float FurLength = Data->FurLength;
			float MinFurLength = Data->MinFurLength;
			float ShellBias = Data->ShellBias;
			float HairLengthForceUniformity = Data->HairLengthForceUniformity;
			float NoiseStrength = Data->NoiseStrength;

			volatile bool finished = false;
			ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([Data, &finished](FRHICommandListImmediate& RHICmdList) { Data->~FFurStaticData(); finished = true; });
			while (!finished)
				;
			new (Data) FFurStaticData(StaticMesh, Lod, FurSplines, TArray<UStaticMesh*>(), FurLayerCount, FurLength, MinFurLength, ShellBias, HairLengthForceUniformity, NoiseStrength);
		}
	}

	StaticFurStaticDataCS.Unlock();
}


class FurStaticDataDelayedCleanupTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FurStaticDataDelayedCleanupTask>;

public:
	FurStaticDataDelayedCleanupTask(const TArray<FFurData*>& InFurDataArray) : FurDataArray(InFurDataArray) {}

protected:
	const TArray<FFurData*> FurDataArray;

	void DoWork()
	{
		FPlatformProcess::Sleep(1.0f);

		StaticFurStaticDataCS.Lock();
		for (int i = FurDataArray.Num() - 1; i != -1; i--)
		{
			FFurStaticData* data = (FFurStaticData*)FurDataArray[i];
			if (--data->RefCount == 0)
			{
				StaticFurStaticData.Remove(data);
				ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([data](FRHICommandListImmediate& RHICmdList) { delete data; });
			}
		}
		StaticFurStaticDataCS.Unlock();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTaskName, STATGROUP_ThreadPoolAsyncTasks);
	}
};

FFurData* FFurStaticData::CreateFurData(int InFurLayerCount, int InLod, UGFurComponent* FurComponent)
{
	if (InFurLayerCount < 1)
		InFurLayerCount = 1;
	if (InFurLayerCount > 128)
		InFurLayerCount = 128;

	float FurLengthClamped = FMath::Max(FurComponent->FurLength, 0.001f);

	StaticFurStaticDataCS.Lock();

	FFurStaticData* Data = nullptr;
	for (int32 i = 0; i < StaticFurStaticData.Num(); i++)
	{
		FFurStaticData* d = StaticFurStaticData[i];
		if (d->StaticMesh == FurComponent->StaticGrowMesh && d->Lod == InLod && d->FurLayerCount == InFurLayerCount && d->FurSplines == FurComponent->FurSplines
			&& d->GuideMeshes == FurComponent->StaticGuideMeshes
			&& d->FurLength == FurLengthClamped && d->MinFurLength == FurComponent->MinFurLength
			&& d->ShellBias == FurComponent->ShellBias && d->HairLengthForceUniformity == FurComponent->HairLengthForceUniformity
			&& d->NoiseStrength == FurComponent->NoiseStrength)
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
		Data = new FFurStaticData(FurComponent->StaticGrowMesh, InLod, FurComponent->FurSplines, FurComponent->StaticGuideMeshes, InFurLayerCount, FurLengthClamped,
			FurComponent->MinFurLength, FurComponent->ShellBias, FurComponent->HairLengthForceUniformity, FurComponent->NoiseStrength);
		Data->RefCount = 1;
		StaticFurStaticData.Add(Data);
	}

	StaticFurStaticDataCS.Unlock();

	return Data;
}

void FFurStaticData::DestroyFurData(const TArray<FFurData*>& InFurDataArray)
{
	(new FAutoDeleteAsyncTask<FurStaticDataDelayedCleanupTask>(InFurDataArray))->StartBackgroundTask();
}
