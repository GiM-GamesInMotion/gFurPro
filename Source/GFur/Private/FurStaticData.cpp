// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurStaticData.h"
#include "ShaderParameterUtils.h"
#include "FurComponent.h"
#include "Runtime/Renderer/Public/MeshMaterialShader.h"
#include "Runtime/RHI/Public/RHICommandList.h"

static TArray< FFurStaticData* > FurStaticData;
static FCriticalSection FurStaticDataCS;

/** Vertex Factory Shader Parameters */
class FFurStaticVertexFactoryShaderParameters : public FVertexFactoryShaderParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FFurStaticVertexFactoryShaderParameters, NonVirtual);
public:
	void Bind(const FShaderParameterMap& ParameterMap)
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


	void Serialize(FArchive& Ar)
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
	void GetElementShaderBindings(
		const class FSceneInterface* Scene,
		const class FSceneView* View,
		const class FMeshMaterialShader* Shader,
		const EVertexInputStreamType InputStreamType,
		ERHIFeatureLevel::Type FeatureLevel,
		const class FVertexFactory* VertexFactory,
		const struct FMeshBatchElement& BatchElement,
		class FMeshDrawSingleShaderBindings& ShaderBindings,
		FVertexInputStreamArray& VertexStreams) const;

	uint32 GetSize() const { return sizeof(*this); }

private:
	LAYOUT_FIELD(FShaderParameter, MeshOriginParameter);
	LAYOUT_FIELD(FShaderParameter, MeshExtensionParameter);
	LAYOUT_FIELD(FShaderParameter, FurOffsetPowerParameter);
	LAYOUT_FIELD(FShaderParameter, FurLinearOffsetParameter);
	LAYOUT_FIELD(FShaderParameter, FurPositionParameter);
	LAYOUT_FIELD(FShaderParameter, FurAngularOffsetParameter);
	LAYOUT_FIELD(FShaderParameter, PreviousFurLinearOffsetParameter);
	LAYOUT_FIELD(FShaderParameter, PreviousFurPositionParameter);
	LAYOUT_FIELD(FShaderParameter, PreviousFurAngularOffsetParameter);
};

/** Vertex Factory */
template<bool Physics>
class FFurStaticVertexFactoryBase : public FFurVertexFactory
{
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
			: MeshOrigin(0, 0, 0)
			, MeshExtension(1, 1, 1)
			, FurOffsetPower(2.0f)
			, FurLinearOffset(0, 0, 0)
			, FurPosition(0, 0, 0)
			, FurAngularOffset(0, 0, 0)
			, PreviousFurLinearOffset(0, 0, 0)
			, PreviousFurPosition(0, 0, 0)
			, PreviousFurAngularOffset(0, 0, 0)
			, Discontinuous(true)
		{
		}

		void GoToNextFrame(bool InDiscontinuous);

		bool IsPreviousDataValid()
		{
			return !Discontinuous;
		}

	private:
		bool Discontinuous;
	};

	FFurStaticVertexFactoryBase(ERHIFeatureLevel::Type InFeatureLevel)
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

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
	void Init(const FFurVertexBuffer* VertexBuffer)
	{
		typedef FFurStaticVertex<TangentBasisTypeT, UVTypeT> VertexType;
		ENQUEUE_RENDER_COMMAND(InitProceduralMeshVertexFactory)(
			[VertexBuffer, this](FRHICommandListImmediate& RHICmdList) {
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
				NewData.FurOffset = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, VertexType, FurOffset, VET_Float3);

				SetData(NewData);
			});
	}

	static FVertexFactoryShaderParameters* ConstructShaderParameters(EShaderFrequency ShaderFrequency)
	{
		return (ShaderFrequency == SF_Vertex) ? new FFurStaticVertexFactoryShaderParameters() : NULL;
	}


	static bool ShouldCache(const FVertexFactoryShaderPermutationParameters& Parameters)
	{
		return true;
	}

	static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
//		Super::ModifyCompilationEnvironment(Platform, Material, OutEnvironment);
		if (Physics)
			OutEnvironment.SetDefine(TEXT("GFUR_PHYSICS"), TEXT("1"));
	}

	static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
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
		const FVector& InPosition, bool InDiscontinuous, ERHIFeatureLevel::Type InFeatureLevel) override
	{
		ShaderData.GoToNextFrame(InDiscontinuous);

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

class FPhysicsFurStaticVertexFactory : public FFurStaticVertexFactoryBase<true>
{
	DECLARE_VERTEX_FACTORY_TYPE(FPhysicsFurStaticVertexFactory);
public:
	FPhysicsFurStaticVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurStaticVertexFactoryBase<true>(InFeatureLevel)
	{
	}

	using FFurStaticVertexFactoryBase<true>::Init;
};

class FFurStaticVertexFactory : public FFurStaticVertexFactoryBase<false>
{
	DECLARE_VERTEX_FACTORY_TYPE(FFurStaticVertexFactory);
public:
	FFurStaticVertexFactory(ERHIFeatureLevel::Type InFeatureLevel)
		: FFurStaticVertexFactoryBase<false>(InFeatureLevel)
	{
	}

	using FFurStaticVertexFactoryBase<false>::Init;
};

IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FPhysicsFurStaticVertexFactory, SF_Vertex, FFurStaticVertexFactoryShaderParameters);
IMPLEMENT_VERTEX_FACTORY_PARAMETER_TYPE(FFurStaticVertexFactory, SF_Vertex, FFurStaticVertexFactoryShaderParameters);

IMPLEMENT_VERTEX_FACTORY_TYPE(FPhysicsFurStaticVertexFactory, "/Plugin/gFur/Private/GFurStaticFactory.ush", true, false, true, true, false);
IMPLEMENT_VERTEX_FACTORY_TYPE(FFurStaticVertexFactory, "/Plugin/gFur/Private/GFurStaticFactory.ush", true, false, true, true, false);

template<bool Physics>
void FFurStaticVertexFactoryBase<Physics>::FShaderDataType::GoToNextFrame(bool InDiscontinuous)
{
	Discontinuous = InDiscontinuous;
}

void FFurStaticVertexFactoryShaderParameters::GetElementShaderBindings(
	const class FSceneInterface* Scene,
	const class FSceneView* View,
	const class FMeshMaterialShader* Shader,
	const EVertexInputStreamType InputStreamType,
	ERHIFeatureLevel::Type FeatureLevel,
	const class FVertexFactory* VertexFactory,
	const struct FMeshBatchElement& BatchElement,
	class FMeshDrawSingleShaderBindings& ShaderBindings,
	FVertexInputStreamArray& VertexStreams) const
{
	FFurStaticVertexFactory::FShaderDataType& ShaderData = ((FFurStaticVertexFactory*)VertexFactory)->ShaderData;

	ShaderBindings.Add(MeshOriginParameter, ShaderData.MeshOrigin);
	ShaderBindings.Add(MeshExtensionParameter, ShaderData.MeshExtension);
	ShaderBindings.Add(FurOffsetPowerParameter, ShaderData.FurOffsetPower);
	ShaderBindings.Add(FurLinearOffsetParameter, ShaderData.FurLinearOffset);
	ShaderBindings.Add(FurPositionParameter, ShaderData.FurPosition);
	ShaderBindings.Add(FurAngularOffsetParameter, ShaderData.FurAngularOffset);

	if (ShaderData.IsPreviousDataValid())
	{
		ShaderBindings.Add(PreviousFurLinearOffsetParameter, ShaderData.PreviousFurLinearOffset);
		ShaderBindings.Add(PreviousFurPositionParameter, ShaderData.PreviousFurPosition);
		ShaderBindings.Add(PreviousFurAngularOffsetParameter, ShaderData.PreviousFurAngularOffset);
	}
	else
	{
		ShaderBindings.Add(PreviousFurLinearOffsetParameter, ShaderData.FurLinearOffset);
		ShaderBindings.Add(PreviousFurPositionParameter, ShaderData.FurPosition);
		ShaderBindings.Add(PreviousFurAngularOffsetParameter, ShaderData.FurAngularOffset);
	}
}

/** Fur Skin Data */
FFurStaticData* FFurStaticData::CreateFurData(int32 InFurLayerCount, int32 InLod, UGFurComponent* InFurComponent)
{
	check(InFurLayerCount >= MinimalFurLayerCount && InFurLayerCount <= MaximalFurLayerCount);

	FScopeLock lock(&FurStaticDataCS);

	for (FFurStaticData* Data : FurStaticData)
	{
		if (Data->Compare(InFurLayerCount, InLod, InFurComponent))
		{
			Data->RefCount++;
			return Data;
		}
	}
/*	for (FFurStaticData* Data : FurStaticData)
	{
		if (Data->RefCount == 0 && Data->Similar(InLod, InFurComponent))
		{
			Data->Set(InFurLayerCount, InLod, InFurComponent);
			Data->BuildFur(BuildType::Minimal);
			Data->RefCount++;
			return Data;
		}
	}*/

	FFurStaticData* Data = new FFurStaticData();
	Data->Set(InFurLayerCount, InLod, InFurComponent);
	Data->BuildFur(BuildType::Full);
	FurStaticData.Add(Data);
	return Data;
}

void FFurStaticData::DestroyFurData(const TArray<FFurData*>& InFurDataArray)
{
	FScopeLock lock(&FurStaticDataCS);

	for (auto* Data : InFurDataArray)
		((FFurStaticData*)Data)->RefCount--;

	StartFurDataCleanupTask([]() {

		FScopeLock lock(&FurStaticDataCS);

		for (int32 i = FurStaticData.Num() - 1; i >= 0; i--)
		{
			FFurStaticData* Data = FurStaticData[i];
			if (Data->RefCount == 0)
			{
				FurStaticData.RemoveAt(i);
				ENQUEUE_RENDER_COMMAND(ReleaseDataCommand)([Data](FRHICommandListImmediate& RHICmdList) { delete Data; });
			}
		}
	});
}

void FFurStaticData::CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel)
{
	auto CreateVertexFactory = [&](const FFurData::FSection& s, auto* vf) {
		if (bUseHighPrecisionTangentBasis)
		{
			if (bUseFullPrecisionUVs)
				vf->template Init<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::HighPrecision>(&VertexBuffer);
			else
				vf->template Init<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::Default>(&VertexBuffer);
		}
		else
		{
			if (bUseFullPrecisionUVs)
				vf->template Init<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::HighPrecision>(&VertexBuffer);
			else
				vf->template Init<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default>(&VertexBuffer);
		}
		BeginInitResource(vf);
		VertexFactories.Add(vf);
	};

	if (InPhysics)
	{
		for (auto& s : Sections)
		{
			CreateVertexFactory(s, new FPhysicsFurStaticVertexFactory(InFeatureLevel));
		}
	}
	else
	{
		for (auto& s : Sections)
		{
			CreateVertexFactory(s, new FFurStaticVertexFactory(InFeatureLevel));
		}
	}
}

FFurStaticData::~FFurStaticData()
{
	UnbindChangeDelegates();

#if WITH_EDITORONLY_DATA
	if (StaticMesh)
		StaticMesh->RemoveFromRoot();
	for (UStaticMesh* Mesh : GuideMeshes)
		Mesh->RemoveFromRoot();
#endif // WITH_EDITORONLY_DATA
}

void FFurStaticData::UnbindChangeDelegates()
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
	if (StaticMeshChangeHandle.IsValid())
	{
		StaticMesh->OnMeshChanged.Remove(StaticMeshChangeHandle);
		StaticMeshChangeHandle.Reset();
	}
	for (int32 i = 0; i < GuideMeshes.Num(); i++)
	{
		if (GuideMeshes[i])
			GuideMeshes[i]->OnMeshChanged.Remove(GuideMeshesChangeHandles[i]);
	}
	GuideMeshesChangeHandles.Reset();
#endif // WITH_EDITORONLY_DATA
}

void FFurStaticData::Set(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent)
{
	UnbindChangeDelegates();
#if WITH_EDITORONLY_DATA
	if (StaticMesh)
		StaticMesh->RemoveFromRoot();
	for (UStaticMesh* Mesh : GuideMeshes)
		Mesh->RemoveFromRoot();
#endif // WITH_EDITORONLY_DATA

	FFurData::Set(InFurLayerCount, InLod, InFurComponent);

	StaticMesh = InFurComponent->StaticGrowMesh;
	GuideMeshes = InFurComponent->StaticGuideMeshes;
#if WITH_EDITORONLY_DATA
	if (StaticMesh)
		StaticMesh->AddToRoot();
	for (auto* Mesh : InFurComponent->SkeletalGuideMeshes)
		Mesh->AddToRoot();
#endif // WITH_EDITORONLY_DATA

	check(StaticMesh);

	if (FurSplinesAssigned == NULL && GuideMeshes.Num() > 0)
	{
		FurSplinesGenerated = NewObject<UFurSplines>();
		GenerateSplines(FurSplinesGenerated, StaticMesh, InLod, GuideMeshes);
		FurSplinesUsed = FurSplinesGenerated;
	}
#if WITH_EDITORONLY_DATA
	StaticMeshChangeHandle = StaticMesh->OnMeshChanged.AddLambda([this]() { BuildFur(BuildType::Full); });
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
				auto Handle = GuideMesh->OnMeshChanged.AddLambda([this, InLod]() {
					if (FurSplinesGenerated)
						FurSplinesGenerated->ConditionalBeginDestroy();
					FurSplinesGenerated = NewObject<UFurSplines>();
					GenerateSplines(FurSplinesGenerated, StaticMesh, InLod, GuideMeshes);
					FurSplinesUsed = FurSplinesGenerated;
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

bool FFurStaticData::Compare(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent)
{
	return FFurData::Compare(InFurLayerCount, InLod, InFurComponent) && StaticMesh == InFurComponent->StaticGrowMesh && GuideMeshes == InFurComponent->StaticGuideMeshes;
}

bool FFurStaticData::Similar(int32 InLod, class UGFurComponent* InFurComponent)
{
	return FFurData::Similar(InLod, InFurComponent) && StaticMesh == InFurComponent->StaticGrowMesh && GuideMeshes == InFurComponent->StaticGuideMeshes;
}

void FFurStaticData::BuildFur(BuildType Build)
{
	auto* StaticMeshResource = StaticMesh->GetRenderData();
	check(StaticMeshResource);

	const FStaticMeshLODResources& LodRenderData = StaticMeshResource->LODResources[Lod];
	if (LodRenderData.VertexBuffers.StaticMeshVertexBuffer.GetUseHighPrecisionTangentBasis())
		BuildFur<EStaticMeshVertexTangentBasisType::HighPrecision>(LodRenderData, Build);
	else
		BuildFur<EStaticMeshVertexTangentBasisType::Default>(LodRenderData, Build);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
inline void FFurStaticData::BuildFur(const FStaticMeshLODResources& LodRenderData, BuildType Build)
{
	if (LodRenderData.VertexBuffers.StaticMeshVertexBuffer.GetUseFullPrecisionUVs())
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::HighPrecision>(LodRenderData, Build);
	else
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::Default>(LodRenderData, Build);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
inline void FFurStaticData::BuildFur(const FStaticMeshLODResources& LodRenderData, BuildType Build)
{
	typedef FFurStaticVertex<TangentBasisTypeT, UVTypeT> VertexType;

	bUseHighPrecisionTangentBasis = TangentBasisTypeT == EStaticMeshVertexTangentBasisType::HighPrecision;
	bUseFullPrecisionUVs = UVTypeT == EStaticMeshVertexUVType::HighPrecision;

	const auto& SourcePositions = LodRenderData.VertexBuffers.PositionVertexBuffer;
	const auto& SourceVertices = LodRenderData.VertexBuffers.StaticMeshVertexBuffer;
	const auto& SourceColors = LodRenderData.VertexBuffers.ColorVertexBuffer;

	const uint32 SourceVertexCount = SourcePositions.GetNumVertices();
	check(SourceVertexCount == SourceVertices.GetNumVertices());

	bool HasVertexColor = SourceColors.GetNumVertices() > 0;
	check(!HasVertexColor || SourceVertexCount == SourceColors.GetNumVertices());

	if (Build == BuildType::Full)
	{
		UnpackNormals<TangentBasisTypeT>(SourceVertices);
	}
	if (Build >= BuildType::Splines)
		GenerateSplineMap(SourcePositions);

	uint32 NewVertexCount = VertexCountPerLayer * FurLayerCount;

	FFurStaticVertexBlitter<TangentBasisTypeT, UVTypeT> VertexBlitter(SourcePositions, SourceVertices, SourceColors);

	while (RenderThreadDataSubmissionPending)
		;

	VertexType* Vertices = VertexBuffer.Lock<VertexType>(NewVertexCount);
	{
		uint32 VertexCount2 = GenerateFurVertices(0, SourceVertexCount, Vertices, VertexBlitter);
		if (Build == BuildType::Full)
		{
			float MaxDistSq = 0;
			for (uint32 i = 0; i < VertexCount2; i++)
			{
				const auto& Position = Vertices[i].Position;
				float d = Position.SizeSquared();
				if (d > MaxDistSq)
					MaxDistSq = d;
			}
			MaxVertexBoneDistance = sqrtf(MaxDistSq);
		}
	}
	VertexBuffer.Unlock();

	if (Build >= BuildType::Splines || FurLayerCount != OldFurLayerCount || RemoveFacesWithoutSplines != OldRemoveFacesWithoutSplines)
	{
		OldFurLayerCount = FurLayerCount;
		OldRemoveFacesWithoutSplines = RemoveFacesWithoutSplines;

		// indices
		TArray<uint32> SourceIndices;
		LodRenderData.IndexBuffer.GetCopy(SourceIndices);

		TArray<FSection>& LocalSections = Sections.Num() ? TempSections : Sections;
		LocalSections.SetNum(LodRenderData.Sections.Num());

		auto& Indices = IndexBuffer.Lock();
		Indices.Reset();
		Indices.AddUninitialized(SourceIndices.Num() * FurLayerCount);
		uint32 Idx = 0;
		for (int32 SectionIndex = 0; SectionIndex < LodRenderData.Sections.Num(); SectionIndex++)
		{
			const auto& SourceSection = LodRenderData.Sections[SectionIndex];
			FSection& FurSection = LocalSections[SectionIndex];

			FurSection.MaterialIndex = SourceSection.MaterialIndex;
			FurSection.MinVertexIndex = 0;
			FurSection.MaxVertexIndex = NewVertexCount - 1;
			FurSection.BaseIndex = Idx;

			for (int32 Layer = 0; Layer < FurLayerCount; ++Layer)
			{
				int32 VertexIndexOffset = Layer * VertexCountPerLayer;
				check(VertexIndexOffset >= 0);
				if (FurSplinesUsed && RemoveFacesWithoutSplines)
				{
					for (uint32 t = 0; t < SourceSection.NumTriangles; ++t)
					{
						uint32 Idx0 = SourceIndices[SourceSection.FirstIndex + t * 3];
						uint32 Idx1 = SourceIndices[SourceSection.FirstIndex + t * 3 + 1];
						uint32 Idx2 = SourceIndices[SourceSection.FirstIndex + t * 3 + 2];
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
					for (uint32 i = 0; i < SourceSection.NumTriangles * 3; ++i)
						Indices[Idx++] = SourceIndices[SourceSection.FirstIndex + i] + VertexIndexOffset;
				}
			}
			FurSection.NumTriangles = (Idx - FurSection.BaseIndex) / 3;
			FurSection.NumBones = 0;
		}
		check(Idx <= (uint32)Indices.Num());
		Indices.RemoveAt(Idx, Indices.Num() - Idx, false);

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

		IndexBuffer.Unlock();
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

void FFurStaticData::BuildFur(const TArray<uint32>& InVertexSet)
{
	auto* StaticMeshResource = StaticMesh->GetRenderData();
	check(StaticMeshResource);

	const FStaticMeshLODResources& LodRenderData = StaticMeshResource->LODResources[Lod];
	if (LodRenderData.VertexBuffers.StaticMeshVertexBuffer.GetUseHighPrecisionTangentBasis())
		BuildFur<EStaticMeshVertexTangentBasisType::HighPrecision>(LodRenderData, InVertexSet);
	else
		BuildFur<EStaticMeshVertexTangentBasisType::Default>(LodRenderData, InVertexSet);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
void FFurStaticData::BuildFur(const FStaticMeshLODResources& LodRenderData, const TArray<uint32>& InVertexSet)
{
	if (LodRenderData.VertexBuffers.StaticMeshVertexBuffer.GetUseFullPrecisionUVs())
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::HighPrecision>(InVertexSet);
	else
		BuildFur<TangentBasisTypeT, EStaticMeshVertexUVType::Default>(InVertexSet);
}

template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
void FFurStaticData::BuildFur(const TArray<uint32>& InVertexSet)
{
	typedef FFurStaticVertex<TangentBasisTypeT, UVTypeT> VertexType;

	while (RenderThreadDataSubmissionPending)
		;

	TArray<float> FurLengths;
	GenerateFurLengths(FurLengths);

	VertexType* Vertices = VertexBuffer.Lock<VertexType>(VertexCountPerLayer * FurLayerCount);
	bool UseRemap = VertexRemap.Num() > 0;
	for (int32 Layer = 0; Layer < FurLayerCount; Layer++)
	{
		auto GenLayerData = CalcFurGenLayerData(FurLayerCount - Layer);
		for (uint32 SrcVertexIndex : InVertexSet)
		{
			VertexType& Vertex = Vertices[(UseRemap ? VertexRemap[SrcVertexIndex] : SrcVertexIndex) + Layer * VertexCountPerLayer];

			if (FurSplinesUsed)
			{
				int32 SplineIndex = SplineMap[SrcVertexIndex];
				float Length = SplineIndex >= 0 ? FurLengths[SplineIndex] : FurLength;
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], Length, GenLayerData, SplineIndex);
			}
			else
			{
				GenerateFurVertex(Vertex.FurOffset, Vertex.UV1, Vertex.UV2, Vertex.UV3, Normals[SrcVertexIndex], FurLength, GenLayerData);
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
void GenerateSplines(UFurSplines* Splines, UStaticMesh* InStaticMesh, int32 InLod, const TArray<UStaticMesh*>& InGuideMeshes)
{
	auto* StaticMeshResource = InStaticMesh->GetRenderData();
	check(StaticMeshResource);

	if (InLod >= StaticMeshResource->LODResources.Num())
		InLod = StaticMeshResource->LODResources.Num() - 1;
	const auto& LodModel = StaticMeshResource->LODResources[InLod];

	const auto& SourcePositions = LodModel.VertexBuffers.PositionVertexBuffer;

	uint32 VertexCount = SourcePositions.GetNumVertices();
	int32 ControlPointCount = InGuideMeshes.Num() + 1;
	Splines->Vertices.AddUninitialized(VertexCount * ControlPointCount);
	for (uint32 i = 0; i < VertexCount; i++)
	{
		int32 Index = i * ControlPointCount;
		Splines->Vertices[Index] = SourcePositions.VertexPosition(i);
	}

	int32 k = 1;
	for (UStaticMesh* GuideMesh : InGuideMeshes)
	{
		if (GuideMesh)
		{
			auto* StaticMeshResource2 = GuideMesh->GetRenderData();
			check(StaticMeshResource2);
			const auto& LodModel2 = StaticMeshResource2->LODResources[InLod];
			const auto& SourcePositions2 = LodModel2.VertexBuffers.PositionVertexBuffer;
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

	Splines->ControlPointCount = ControlPointCount;
	Splines->Version = 2;
}
