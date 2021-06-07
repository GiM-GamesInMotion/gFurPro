// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurComponent.h"
#include "GFur.h"
#include "FurSplines.h"
#include "FurData.h"
#include "FurMorphObject.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"
#include "Runtime/Engine/Public/DynamicMeshBuilder.h"
#include "Runtime/Engine/Public/GPUSkinVertexFactory.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Runtime/Engine/Public/SkeletalRenderPublic.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/SkinnedMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Runtime/Launch/Resources/Version.h"
#include "ShaderParameterUtils.h"
#include "FurSkinData.h"
#include "FurStaticData.h"

#if RHI_RAYTRACING
#include "RayTracingDefinitions.h"
#include "RayTracingInstance.h"
#endif

/** Scene proxy */
class FFurSceneProxy : public FPrimitiveSceneProxy
{
public:
	FFurSceneProxy(UGFurComponent* InComponent, const TArray<FFurData*>& InFurData, const TArray<FFurLod>& InFurLods, const TArray<UMaterialInstanceDynamic*>& InFurMaterials, const TArray<UMaterialInterface*>& InOverrideMaterials, const TArray<FFurMorphObject*>& InMorphObjects, bool InCastShadows, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel)
		: FPrimitiveSceneProxy(InComponent)
		, FurComponent(InComponent)
		, FurData(InFurData)
		, FurLods(InFurLods)
		, FurMaterials(InFurMaterials)
		, FurMorphObjects(InMorphObjects)
		, CastShadows(InCastShadows)
	{
		bAlwaysHasVelocity = true;

		for (int i = 0; i < InOverrideMaterials.Num() && i < FurMaterials.Num(); i++)
		{
			UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(InOverrideMaterials[i]);
			if (DynamicMaterial)
				FurMaterials[i] = DynamicMaterial;
		}

		for (int i = 0; i < InFurData.Num(); i++)
		{
			bool LodPhysics = i > 0 ? InFurLods[i - 1].PhysicsEnabled : true;
			InFurData[i]->CreateVertexFactories(VertexFactories, InMorphObjects[i] ? InMorphObjects[i]->GetVertexBuffer() : NULL, InPhysics && LodPhysics, InFeatureLevel);
		}

#if RHI_RAYTRACING
		if (IsRayTracingEnabled())
		{
			ENQUEUE_RENDER_COMMAND(UpdateDataCommand)([this](FRHICommandListImmediate& RHICmdList) {
				const auto& Sections = FurData[0]->GetSections();
				FRayTracingGeometryInitializer Initializer;
				Initializer.IndexBuffer = FurData[0]->GetIndexBuffer().IndexBufferRHI;
				Initializer.TotalPrimitiveCount = 0;
				Initializer.GeometryType = RTGT_Triangles;
				Initializer.bFastBuild = true;
				Initializer.bAllowUpdate = true;
				for (int sectionIdx = 0; sectionIdx < Sections.Num(); sectionIdx++)
				{
					const FFurData::FSection& Section = Sections[sectionIdx];
					Initializer.TotalPrimitiveCount += Section.NumTriangles;

					FRayTracingGeometrySegment Segment;
					Segment.VertexBuffer = FurData[0]->GetVertexBuffer().VertexBufferRHI;
					Segment.VertexBufferStride = FurData[0]->GetVertexBuffer().GetVertexSize();
					Segment.FirstPrimitive = Section.BaseIndex / 3;
					Segment.NumPrimitives = Section.NumTriangles;
					Initializer.Segments.Add(Segment);

				}
				RayTracingGeometry.SetInitializer(Initializer);
				RayTracingGeometry.InitResource();
			});
		}
#endif
	}

	virtual ~FFurSceneProxy()
	{
		for (auto* VertexFactory : VertexFactories)
		{
			VertexFactory->ReleaseResource();
			delete VertexFactory;
		}
		for (auto* MorphObject : FurMorphObjects)
			delete MorphObject;
#if RHI_RAYTRACING
		RayTracingGeometry.ReleaseResource();
#endif
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap,
		FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_ProceduralMeshSceneProxy_GetDynamicMeshElements);

		const bool Wireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
			FLinearColor(0, 0.5f, 1.f));

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		int NewLodLevel = 0x7fffffff;
		if (FurComponent->LODFromParent)
		{
			const USkinnedMeshComponent* const MasterComp = FurComponent->GetMasterPoseComponent().Get();
			if (MasterComp && MasterComp->SkeletalMesh && MasterComp->MeshObject)
			{
#if WITH_EDITOR
				const int32 LODBias = MasterComp->GetLODBias();
#else
				const int32 LODBias = 0;
#endif
				NewLodLevel = MasterComp->MeshObject->MinDesiredLODLevel + LODBias;
			}
		}
		else
		{
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
		}

		bool FirstFrame = LastFrameNumber == 0;
		if (ViewFamily.FrameNumber != LastFrameNumber)
		{
			LastFurLodLevel = CurrentFurLodLevel;
			LastMeshLodLevel = CurrentMeshLodLevel;
			LastSectionOffset = SectionOffset;
			LastFrameNumber = ViewFamily.FrameNumber;
		}

		NewLodLevel = FMath::Min(NewLodLevel, FurData.Num() - 1);
		if (NewLodLevel != CurrentFurLodLevel)
		{
			CurrentFurLodLevel = NewLodLevel;
			CurrentMeshLodLevel = FurData[CurrentFurLodLevel]->GetLod();
			SectionOffset = 0;
			for (int i = 0; i < NewLodLevel; i++)
				SectionOffset += FurData[i]->GetSections_RenderThread().Num();
		}
/*		if (FirstFrame)
		{
			LastFurLodLevel = CurrentFurLodLevel;
			LastMeshLodLevel = CurrentMeshLodLevel;
			LastSectionOffset = SectionOffset;
		}*/

		if (LastFurLodLevel < FurData.Num())
		{
			const auto& Sections = FurData[LastFurLodLevel]->GetSections_RenderThread();
			for (int sectionIdx = 0; sectionIdx < Sections.Num(); sectionIdx++)
			{
				const FFurData::FSection& section = Sections[sectionIdx];
				if (section.NumTriangles == 0)
					continue;
				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
				{
					if (VisibilityMap & (1 << ViewIndex))
					{
						auto MorphObject = GetMorphObject(false);
						if (MorphObject != nullptr && !MorphObject->GetVertexBuffer()->IsInitialized())
							continue;

						const FSceneView* View = Views[ViewIndex];

						FMaterialRenderProxy* MaterialProxy = NULL;
						if (Wireframe)
						{
							MaterialProxy = WireframeMaterialInstance;
						}
						else
						{
							UMaterialInstanceDynamic* material = FurMaterials[section.MaterialIndex];
							MaterialProxy = material->GetRenderProxy();
						}

						FMeshBatch& Mesh = Collector.AllocateMesh();
						FMeshBatchElement& BatchElement = Mesh.Elements[0];
						BatchElement.IndexBuffer = FurData[LastFurLodLevel]->GetIndexBuffer_RenderThread();
						Mesh.bWireframe = Wireframe;
						Mesh.VertexFactory = GetVertexFactory(sectionIdx, false);
						Mesh.MaterialRenderProxy = MaterialProxy;
						BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
						BatchElement.FirstIndex = section.BaseIndex;
						BatchElement.NumPrimitives = section.NumTriangles;
						BatchElement.MinVertexIndex = section.MinVertexIndex;
						BatchElement.MaxVertexIndex = section.MaxVertexIndex;
						Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
						Mesh.Type = PT_TriangleList;
						Mesh.DepthPriorityGroup = SDPG_World;
						Mesh.bCanApplyViewModeOverrides = true;
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
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		//Material->GetRelevance(GetScene().GetFeatureLevel()).SetPrimitiveViewRelevance(Result);
		Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		return Result;
	}

#if RHI_RAYTRACING
	virtual bool IsRayTracingRelevant() const override { return true; }
	virtual void GetDynamicRayTracingInstances(FRayTracingMaterialGatheringContext& Context, TArray<FRayTracingInstance>& OutRayTracingInstances) override
	{
		const auto& Sections = FurData[0]->GetSections_RenderThread();
		if (RayTracingGeometry.RayTracingGeometryRHI.IsValid())
		{
			FRayTracingInstance RayTracingInstance;
			RayTracingInstance.Geometry = &RayTracingGeometry;
			RayTracingInstance.InstanceTransforms.Add(GetLocalToWorld());

			for (int sectionIdx = 0; sectionIdx < Sections.Num(); sectionIdx++)
			{
				const FFurData::FSection& Section = Sections[sectionIdx];
				check(RayTracingGeometry.Initializer.IndexBuffer.IsValid());

				UMaterialInstanceDynamic* material = FurMaterials[Section.MaterialIndex];
				auto MaterialProxy = material->GetRenderProxy();

				FMeshBatch MeshBatch;

				MeshBatch.VertexFactory = GetVertexFactory(sectionIdx, false);
				MeshBatch.SegmentIndex = sectionIdx;
				MeshBatch.MaterialRenderProxy = MaterialProxy;
				MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
				MeshBatch.Type = PT_TriangleList;
				MeshBatch.DepthPriorityGroup = SDPG_World;
				MeshBatch.CastRayTracedShadow = bCastDynamicShadow;

				FMeshBatchElement& BatchElement = MeshBatch.Elements[0];
				BatchElement.IndexBuffer = FurData[0]->GetIndexBuffer_RenderThread();
				BatchElement.FirstIndex = Section.BaseIndex;
				BatchElement.NumPrimitives = Section.NumTriangles;
				BatchElement.MinVertexIndex = Section.MinVertexIndex;
				BatchElement.MaxVertexIndex = Section.MaxVertexIndex;

				RayTracingInstance.Materials.Add(MeshBatch);
			}
			RayTracingInstance.BuildInstanceMaskAndFlags();
			OutRayTracingInstances.Add(RayTracingInstance);
		}
	}
#endif

	virtual bool CanBeOccluded() const override { return true; }

	virtual uint32 GetMemoryFootprint(void) const { return (sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return (FPrimitiveSceneProxy::GetAllocatedSize()); }

	FFurData* GetFurData(bool Current) { return FurData[FMath::Min(Current ? CurrentFurLodLevel : LastFurLodLevel, FurData.Num() - 1)]; }
	FFurVertexFactory* GetVertexFactory(int sectionIdx, bool Current) const { return VertexFactories[(Current ? SectionOffset : LastSectionOffset) + sectionIdx]; }
	FFurMorphObject* GetMorphObject(bool Current) const { return FurMorphObjects[Current ? CurrentFurLodLevel : LastFurLodLevel]; }

	int GetCurrentFurLodLevel() const { return CurrentFurLodLevel; }
	int GetCurrentMeshLodLevel() const { return CurrentMeshLodLevel; }

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
	TArray<FFurMorphObject*> FurMorphObjects;
	mutable int CurrentFurLodLevel = 0;
	mutable int CurrentMeshLodLevel = 0;
	mutable int SectionOffset = 0;
	mutable int LastFurLodLevel = 0;
	mutable int LastMeshLodLevel = 0;
	mutable int LastSectionOffset = 0;
	mutable int LastFrameNumber = 0;
	bool CastShadows;

#if RHI_RAYTRACING
	FRayTracingGeometry RayTracingGeometry;
#endif
};

//////////////////////////////////////////////////////////////////////////

UGFurComponent::UGFurComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickInEditor = true;
	bAutoActivate = true;
	LayerCount = 32;
	ShellBias = 1.0f;
	FurLength = 1.0f;
	MinFurLength = 0.0f;
	RemoveFacesWithoutSplines = false;
	PhysicsEnabled = true;
	ForceDistribution = 2.0f;
	Stiffness = 5.0f;
	Damping = 5.0f;
	MaxForce = 10.0f;
	MaxForceTorqueFactor = 0.75f;
	ConstantForce.Set(0, 0, -9.8f);
	ReferenceHairBias = 0.8f;
	HairLengthForceUniformity = 0.75f;
	MaxPhysicsOffsetLength = FLT_MAX;
	NoiseStrength = 0.0f;
	CastShadow = false;
	PrimaryComponentTick.bCanEverTick = true;
	DisableMorphTargets = false;

	StreamingDistanceMultiplier = 1.0f;

	LastDeltaTime = 1.0f;

	SetGenerateOverlapEvents(false);
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void UGFurComponent::RegenerateFur()
{
	if (IsRenderStateCreated())
	{
		DestroyRenderState_Concurrent();
		CreateRenderState_Concurrent(nullptr);
	}
}

const TArray<int32>& UGFurComponent::GetFurSplineMap() const
{
	return FurData[0]->GetSplineMap();
}

const TArray<FVector>& UGFurComponent::GetVertexNormals() const
{
	return FurData[0]->GetVertexNormals();
}

UMaterialInterface* UGFurComponent::GetMaterial(int32 MaterialIndex) const
{
	if (MaterialIndex < OverrideMaterials.Num() && OverrideMaterials[MaterialIndex])
	{
		return OverrideMaterials[MaterialIndex];
	}
	else if (SkeletalGrowMesh)
	{
		const auto& Materials = SkeletalGrowMesh->GetMaterials();
		if (MaterialIndex < Materials.Num() && Materials[MaterialIndex].MaterialInterface)
			return Materials[MaterialIndex].MaterialInterface;
	}
	else if (StaticGrowMesh)
	{
		const auto& Materials = StaticGrowMesh->GetStaticMaterials();
		if (MaterialIndex < Materials.Num() && Materials[MaterialIndex].MaterialInterface)
			return Materials[MaterialIndex].MaterialInterface;
	}

	return NULL;
}

int32 UGFurComponent::GetMaterialIndex(FName MaterialSlotName) const
{
	if (SkeletalGrowMesh)
	{
		const auto& Materials = SkeletalGrowMesh->GetMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = Materials[MaterialIndex];
			if (SkeletalMaterial.MaterialSlotName == MaterialSlotName)
			{
				return MaterialIndex;
			}
		}
	}
	else if (StaticGrowMesh)
	{
		const auto& Materials = StaticGrowMesh->GetStaticMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = Materials[MaterialIndex];
			if (SkeletalMaterial.MaterialSlotName == MaterialSlotName)
			{
				return MaterialIndex;
			}
		}
	}
	return -1;
}

TArray<FName> UGFurComponent::GetMaterialSlotNames() const
{
	TArray<FName> MaterialNames;
	if (SkeletalGrowMesh)
	{
		const auto& Materials = SkeletalGrowMesh->GetMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = Materials[MaterialIndex];
			MaterialNames.Add(SkeletalMaterial.MaterialSlotName);
		}
	}
	else if (StaticGrowMesh)
	{
		const auto& Materials = StaticGrowMesh->GetStaticMaterials();
		for (int32 MaterialIndex = 0; MaterialIndex < Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = Materials[MaterialIndex];
			MaterialNames.Add(SkeletalMaterial.MaterialSlotName);
		}
	}
	return MaterialNames;
}

bool UGFurComponent::IsMaterialSlotNameValid(FName MaterialSlotName) const
{
	return GetMaterialIndex(MaterialSlotName) >= 0;
}

void UGFurComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* Material)
{
	if (ElementIndex >= 0)
	{
		if (OverrideMaterials.IsValidIndex(ElementIndex) && (OverrideMaterials[ElementIndex] == Material))
		{
			// Do nothing, the material is already set
		}
		else
		{
			// Grow the array if the new index is too large
			if (OverrideMaterials.Num() <= ElementIndex)
			{
				OverrideMaterials.AddZeroed(ElementIndex + 1 - OverrideMaterials.Num());
			}

			// Check if we are setting a dynamic instance of the original material, or replacing a nullptr material  (if not we should dirty the material parameter name cache)
			if (Material != nullptr)
			{
				UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
				if ((DynamicMaterial != nullptr && DynamicMaterial->Parent != OverrideMaterials[ElementIndex]) || OverrideMaterials[ElementIndex] == nullptr)
				{
					// Mark cached material parameter names dirty
					MarkCachedMaterialParameterNameIndicesDirty();
				}
			}

			// Set the material and invalidate things
			OverrideMaterials[ElementIndex] = Material;
			MarkRenderStateDirty();
			if (Material)
			{
				Material->AddToCluster(this, true);
			}

			FBodyInstance* BodyInst = GetBodyInstance();
			if (BodyInst && BodyInst->IsValidBodyInstance())
			{
				BodyInst->UpdatePhysicalMaterials();
			}
		}
	}
}

void UGFurComponent::SetMaterialByName(FName MaterialSlotName, class UMaterialInterface* Material)
{
	int32 MaterialIndex = GetMaterialIndex(MaterialSlotName);
	if (MaterialIndex < 0)
		return;

	SetMaterial(MaterialIndex, Material);
}

void UGFurComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	if (SkeletalGrowMesh)
	{
		const auto& Materials = SkeletalGrowMesh->GetMaterials();
		// The max number of materials used is the max of the materials on the skeletal mesh and the materials on the mesh component
		const int32 NumMaterials = FMath::Max(Materials.Num(), OverrideMaterials.Num());
		for (int32 MatIdx = 0; MatIdx < NumMaterials; ++MatIdx)
		{
			// GetMaterial will determine the correct material to use for this index.  
			UMaterialInterface* MaterialInterface = GetMaterial(MatIdx);
			OutMaterials.Add(MaterialInterface);
		}

		for (UMaterialInstanceDynamic* material : FurMaterials)
			OutMaterials.Add(material);
	}
	else if (StaticGrowMesh)
	{
		const auto& Materials = StaticGrowMesh->GetStaticMaterials();
		// The max number of materials used is the max of the materials on the skeletal mesh and the materials on the mesh component
		const int32 NumMaterials = FMath::Max(Materials.Num(), OverrideMaterials.Num());
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
	if (SkeletalGrowMesh)
	{
		MaterialData.Material = GetMaterial(MaterialIndex);
		MaterialData.UVChannelData = SkeletalGrowMesh->GetUVChannelData(MaterialIndex);
		MaterialData.PackedRelativeBox = PackedRelativeBox_Identity;
	}
	else if (StaticGrowMesh)
	{
		MaterialData.Material = GetMaterial(MaterialIndex);
		MaterialData.UVChannelData = StaticGrowMesh->GetUVChannelData(MaterialIndex);
		MaterialData.PackedRelativeBox = PackedRelativeBox_Identity;
	}
	return MaterialData.IsValid();
}

void UGFurComponent::GetStreamingRenderAssetInfo(FStreamingTextureLevelContext& LevelContext, TArray<FStreamingRenderAssetPrimitiveInfo>& OutStreamingRenderAssets) const
{
	GetStreamingTextureInfoInner(LevelContext, nullptr, GetComponentTransform().GetMaximumAxisScale() * StreamingDistanceMultiplier, OutStreamingRenderAssets);
}

int32 UGFurComponent::GetNumMaterials() const
{
	if (SkeletalGrowMesh)
	{
		return SkeletalGrowMesh->GetMaterials().Num();
	}
	else if (StaticGrowMesh)
	{
		return StaticGrowMesh->GetStaticMaterials().Num();
	}

	return 0;
}


FPrimitiveSceneProxy* UGFurComponent::CreateSceneProxy()
{
//	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
//	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
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

		MorphRemapTables.Reset();

		TArray<FFurData*> FurArray;
		TArray<FFurMorphObject*> MorphObjects;
		if (SkeletalGrowMesh && SkeletalGrowMesh->GetResourceForRendering())
		{
			UpdateMasterBoneMap();

			auto NumLods = SkeletalGrowMesh->GetResourceForRendering()->LODRenderData.Num();
			MorphRemapTables.SetNum(NumLods);

			bool UseMorphTargets = !DisableMorphTargets && MasterPoseComponent.IsValid() && MasterPoseComponent->SkeletalMesh->GetMorphTargets().Num() > 0;

			{
				auto Data = FFurSkinData::CreateFurData(FMath::Max(LayerCount, 1), 0, this);
				FurArray.Add(Data);
				MorphObjects.Add(UseMorphTargets ? new FFurMorphObject(Data) : NULL);
				if (UseMorphTargets)
					CreateMorphRemapTable(0);
			}
			for (FFurLod& lod : LODs)
			{
				auto Data = FFurSkinData::CreateFurData(FMath::Max(lod.LayerCount, 1), FMath::Min(NumLods - 1, lod.Lod), this);
				if (!lod.DisableMorphTargets && UseMorphTargets)
					CreateMorphRemapTable(FMath::Min(NumLods - 1, lod.Lod));
				FurArray.Add(Data);
				MorphObjects.Add(!lod.DisableMorphTargets && UseMorphTargets ? new FFurMorphObject(Data) : NULL);
			}

			FurData = FurArray;

			return new FFurSceneProxy(this, FurData, LODs, FurMaterials, OverrideMaterials, MorphObjects, CastShadow, PhysicsEnabled, GetWorld()->FeatureLevel);
		}
		else if (StaticGrowMesh && StaticGrowMesh->GetRenderData())
		{
			FurArray.Add(FFurStaticData::CreateFurData(FMath::Max(LayerCount, 1), 0, this));
			MorphObjects.Add(NULL);
			for (FFurLod& lod : LODs)
			{
				FurArray.Add(FFurStaticData::CreateFurData(FMath::Max(lod.LayerCount, 1), FMath::Min(StaticGrowMesh->GetRenderData()->LODResources.Num() - 1, lod.Lod), this));
				MorphObjects.Add(NULL);
			}

			FurData = FurArray;
			return new FFurSceneProxy(this, FurData, LODs, FurMaterials, OverrideMaterials, MorphObjects, CastShadow, PhysicsEnabled, GetWorld()->FeatureLevel);
		}
	}
	return nullptr;
}


void UGFurComponent::CreateRenderState_Concurrent(FRegisterComponentContext* Context)
{
//	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
//	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
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

	Super::CreateRenderState_Concurrent(Context);

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

//	ERHIFeatureLevel::Type FeatureLevel = GetWorld()->FeatureLevel;
//	if (FeatureLevel >= ERHIFeatureLevel::ES3_1)
	{
		for (auto& mat : FurMaterials)
			mat->RemoveFromRoot();
		FurMaterials.Reset();

		if (SkeletalGrowMesh)
			FFurSkinData::DestroyFurData(FurData);
		else if (StaticGrowMesh)
			FFurStaticData::DestroyFurData(FurData);
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
	if (SkeletalGrowMesh)
	{
		if (MasterPoseComponent.IsValid())
		{
			FBoxSphereBounds MasterBounds = MasterPoseComponent->CalcBounds(LocalToWorld);
			MasterBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
			return MasterBounds;
		}
		FBoxSphereBounds DummyBounds = SkeletalGrowMesh->GetBounds();
		DummyBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
		return DummyBounds.TransformBy(LocalToWorld);
	}
	else if (StaticGrowMesh)
	{
		FBoxSphereBounds MeshBounds = StaticGrowMesh->GetBounds();
		MeshBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
		return MeshBounds.TransformBy(LocalToWorld);
	}
	FBoxSphereBounds DummyBounds = FBoxSphereBounds(FVector(0, 0, 0), FVector(0, 0, 0), 0);
	DummyBounds.ExpandBy(FMath::Max(FurLength, 0.001f));
	return DummyBounds.TransformBy(LocalToWorld);
}


UBodySetup* UGFurComponent::GetBodySetup()
{
	return nullptr;
}


void UGFurComponent::updateFur()
{
	if (!SceneProxy || (!SkeletalGrowMesh && !StaticGrowMesh))
		return;

	FFurSceneProxy* Scene = (FFurSceneProxy*)SceneProxy;
	int32 FurLodLevel = Scene->GetCurrentFurLodLevel();

	bool LodPhysicsEnabled = PhysicsEnabled && (FurLodLevel == 0 || LODs[FurLodLevel - 1].PhysicsEnabled);

	float DeltaTime = fminf(LastDeltaTime, 1.0f);
	float ReferenceFurLength = FMath::Max(0.00001f, Scene->GetFurData(true)->GetCurrentMaxFurLength() * ReferenceHairBias + Scene->GetFurData(true)->GetCurrentMinFurLength() * (1.0f - ReferenceHairBias));
	//	float ForceFactor = 1.0f / (powf(ReferenceFurLength, FurForcePower) * fmaxf(FurStiffness, 0.000001f));
	float ForceFactor = 1.0f / powf(ReferenceFurLength, ForceDistribution);
	float DampingClamped = fmaxf(Damping, 0.000001f);
	float DampingFactor = powf(1.0f - (DampingClamped / (DampingClamped + 1.0f)), DeltaTime);
	float MaxForceFinal = (MaxForce * ReferenceFurLength) / powf(ReferenceFurLength, ForceDistribution);
	float MaxTorque = MaxForceTorqueFactor * MaxForceFinal / Scene->GetFurData(true)->GetMaxVertexBoneDistance();
	//	FVector FurForceFinal = FurForce * (fmaxf(FurWeight, 0.000001f) * ForceFactor);
	FVector FurForceFinal = ConstantForce * ReferenceFurLength * ForceFactor / Stiffness;

	float x = DeltaTime * Stiffness;

	FMatrix ToWorld = GetComponentTransform().ToMatrixNoScale();

	auto Physics = [&](const FMatrix& NewTransformation, FMatrix& Transformation, FVector& LinearOffset, FVector& LinearVelocity
		, FVector& AngularOffset, FVector& AngularVelocity) {
		FVector d = (NewTransformation.GetOrigin() - Transformation.GetOrigin());
		d *= ForceFactor;
		LinearOffset -= d;

		FVector force;
		FVector newOffset = (LinearVelocity * FMath::Sin(x) + (LinearOffset - FurForceFinal) * FMath::Cos(x)) * DampingFactor + FurForceFinal;
		FVector newVelocity = (LinearVelocity * FMath::Cos(x) - (LinearOffset - FurForceFinal) * FMath::Sin(x)) * DampingFactor;
		check(newOffset.X == newOffset.X && newOffset.Y == newOffset.Y && newOffset.Z == newOffset.Z);
		check(newVelocity.X == newVelocity.X && newVelocity.Y == newVelocity.Y && newVelocity.Z == newVelocity.Z);
		LinearOffset = newOffset;
		LinearVelocity = newVelocity;
		if (LinearOffset.Size() > MaxForceFinal)
		{
			LinearOffset *= MaxForceFinal / LinearOffset.Size();
			float k = FVector::DotProduct(LinearOffset, LinearVelocity) / FVector::DotProduct(LinearOffset, LinearOffset);
			if (k > 0.0f)
				LinearVelocity -= LinearOffset * k;
		}

		FQuat rdiff = NewTransformation.ToQuat() * Transformation.ToQuat().Inverse();
		float angle;
		rdiff.ToAxisAndAngle(d, angle);
		if (angle > PI)
			angle -= 2 * PI;
		d *= -angle * ForceFactor;
		AngularOffset -= d;
		newOffset = (AngularVelocity * FMath::Sin(x) + AngularOffset * FMath::Cos(x)) * DampingFactor;
		newVelocity = (AngularVelocity * FMath::Cos(x) - AngularOffset * FMath::Sin(x)) * DampingFactor;
		AngularOffset = newOffset;
		AngularVelocity = newVelocity;
		if (AngularOffset.Size() > MaxTorque)
			AngularOffset *= MaxTorque / AngularOffset.Size();

		Transformation = NewTransformation;
	};

	if (SkeletalGrowMesh)
	{
		const USkeletalMesh* const ThisMesh = SkeletalGrowMesh;
		const USkinnedMeshComponent* const MasterComp = MasterPoseComponent.Get();
		const USkeletalMesh* const MasterCompMesh = MasterComp ? MasterComp->SkeletalMesh : nullptr;
		const auto& LOD = SkeletalGrowMesh->GetResourceForRendering()->LODRenderData[Scene->GetCurrentMeshLodLevel()];

		TArray<FMatrix, TInlineAllocator<256>> TempMatrices;
		TArray<bool, TInlineAllocator<256>> ValidTempMatrices;
		const auto& RefSkeleton = ThisMesh->GetRefSkeleton();
		const auto& RefBasesInvMatrix = ThisMesh->GetRefBasesInvMatrix();
		check(RefBasesInvMatrix.Num() != 0);
		if (ReferenceToLocal.Num() != RefBasesInvMatrix.Num())
		{
			Transformations.Reset();
			Transformations.AddUninitialized(RefBasesInvMatrix.Num());
			ReferenceToLocal.Reset();
			ReferenceToLocal.AddUninitialized(RefBasesInvMatrix.Num());
			LinearVelocities.Reset();
			LinearVelocities.AddUninitialized(RefBasesInvMatrix.Num());
			AngularVelocities.Reset();
			AngularVelocities.AddUninitialized(RefBasesInvMatrix.Num());
			LinearOffsets.Reset();
			LinearOffsets.AddUninitialized(RefBasesInvMatrix.Num());
			AngularOffsets.Reset();
			AngularOffsets.AddUninitialized(RefBasesInvMatrix.Num());
			OldPositionValid = false;
		}

		ValidTempMatrices.AddDefaulted(ReferenceToLocal.Num());
		TempMatrices.AddUninitialized(ReferenceToLocal.Num());

		int32 SyncLODLevel = 0;
		if (MasterComp && MasterComp->SkeletalMesh && MasterComp->MeshObject)
		{
#if WITH_EDITOR
			const int32 LODBias = MasterComp->GetLODBias();
#else
			const int32 LODBias = 0;
#endif
			SyncLODLevel = MasterComp->MeshObject->MinDesiredLODLevel + LODBias;
		}

		const auto& CurrentMasterBoneMap = MasterBoneMap[FMath::Clamp(SyncLODLevel, 0, MasterBoneMap.Num() - 1)];

		const bool bIsMasterCompValid = MasterComp && CurrentMasterBoneMap.Num() == RefSkeleton.GetNum();

		const TArray<FBoneIndexType>* RequiredBoneSets[3] = { &LOD.ActiveBoneIndices, 0/*ExtraRequiredBoneIndices*/, NULL };

		// Handle case of using ParentAnimComponent for SpaceBases.
		for (int32 RequiredBoneSetIndex = 0; RequiredBoneSets[RequiredBoneSetIndex] != NULL; RequiredBoneSetIndex++)
		{
			const TArray<FBoneIndexType>& RequiredBoneIndices = *RequiredBoneSets[RequiredBoneSetIndex];
			auto Cnt = FMath::Max(RequiredBoneIndices.Num(), RequiredBoneIndices.Num() ? RequiredBoneIndices.Last() + 1 : 0);
			if (Cnt > ValidTempMatrices.Num())
			{
				auto Count = Cnt - ValidTempMatrices.Num();
				ValidTempMatrices.AddDefaulted(Count);
				TempMatrices.AddUninitialized(Count);
			}

			// Get the index of the bone in this skeleton, and loop up in table to find index in parent component mesh.
			for (int32 BoneIndex = 0; BoneIndex < RequiredBoneIndices.Num(); BoneIndex++)
			{
				const int32 ThisBoneIndex = RequiredBoneIndices[BoneIndex];
				if (ThisBoneIndex >= ValidTempMatrices.Num())
				{
					auto Count = ThisBoneIndex - ValidTempMatrices.Num() + 1;
					ValidTempMatrices.AddDefaulted(Count);
					TempMatrices.AddUninitialized(Count);
				}

				if (RefBasesInvMatrix.IsValidIndex(ThisBoneIndex))
				{
					// On the off chance the parent matrix isn't valid, revert to identity.
					TempMatrices[ThisBoneIndex] = FMatrix::Identity;

					if (bIsMasterCompValid)
					{
						// If valid, use matrix from parent component.
						const int32 MasterBoneIndex = CurrentMasterBoneMap[ThisBoneIndex];
						if (MasterComp->GetComponentSpaceTransforms().IsValidIndex(MasterBoneIndex))
						{
							const int32 ParentIndex = RefSkeleton.GetParentIndex(ThisBoneIndex);
							bool bNeedToHideBone = MasterComp->GetBoneVisibilityStates()[MasterBoneIndex] != BVS_Visible;
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
						else
						{
							const int32 ParentIndex = RefSkeleton.GetParentIndex(ThisBoneIndex);
							if (ParentIndex >= 0)
							{
								TempMatrices[ThisBoneIndex] = RefSkeleton.GetRefBonePose()[ThisBoneIndex].ToMatrixWithScale() * TempMatrices[ParentIndex];
								ValidTempMatrices[ThisBoneIndex] = true;
							}
						}
					}
					else
					{
						TempMatrices[ThisBoneIndex] = RefBasesInvMatrix[ThisBoneIndex].Inverse();
						ValidTempMatrices[ThisBoneIndex] = true;
					}
				}
				// removed else statement to set ReferenceToLocal[ThisBoneIndex] = FTransform::Identity;
				// since it failed in ( ThisMesh->RefBasesInvMatrix.IsValidIndex(ThisBoneIndex) ), ReferenceToLocal is not valid either
				// because of the initialization code line above to match both array count
				// if(ReferenceToLocal.Num() != ThisMesh->RefBasesInvMatrix.Num())
			}
		}

		if (OldPositionValid && LodPhysicsEnabled)
		{
			for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ++ThisBoneIndex)
			{
				FMatrix NewTransformation;
				if (ValidTempMatrices[ThisBoneIndex])
				{
					ReferenceToLocal[ThisBoneIndex] = RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
					NewTransformation = TempMatrices[ThisBoneIndex] * ToWorld;
					NewTransformation.RemoveScaling();
				}
				else
				{
					ReferenceToLocal[ThisBoneIndex] = FMatrix::Identity;
					NewTransformation = FMatrix::Identity;
				}

				Physics(NewTransformation, Transformations[ThisBoneIndex], LinearOffsets[ThisBoneIndex], LinearVelocities[ThisBoneIndex]
					, AngularOffsets[ThisBoneIndex], AngularVelocities[ThisBoneIndex]);
			}
		}
		else
		{
			for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ++ThisBoneIndex)
			{
				if (ValidTempMatrices[ThisBoneIndex])
				{
					ReferenceToLocal[ThisBoneIndex] = RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
					Transformations[ThisBoneIndex] = TempMatrices[ThisBoneIndex] * ToWorld;
					Transformations[ThisBoneIndex].RemoveScaling();
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
			OldPositionValid = true;
		}
	}
	else
	{
		check(StaticGrowMesh);
		if (OldPositionValid && LodPhysicsEnabled)
		{
			Physics(ToWorld, StaticTransformation, StaticLinearOffset, StaticLinearVelocity, StaticAngularOffset, StaticAngularVelocity);
		}
		else
		{
			StaticTransformation = ToWorld;

			StaticLinearOffset.Set(0.0f, 0.0f, 0.0f);
			StaticAngularOffset.Set(0.0f, 0.0f, 0.0f);

			StaticLinearVelocity.Set(0.0f, 0.0f, 0.0f);
			StaticAngularVelocity.Set(0.0f, 0.0f, 0.0f);

			OldPositionValid = true;
		}
	}

	// We prepare the next frame but still have the value from the last one
	uint32 RevisionNumber = MasterPoseComponent.IsValid() ? MasterPoseComponent->GetBoneTransformRevisionNumber() : 0;
	bool Discontinuous = RevisionNumber - LastRevisionNumber > 1;
	LastRevisionNumber = RevisionNumber;

	// queue a call to update this data
	ENQUEUE_RENDER_COMMAND(SkelMeshObjectUpdateDataCommand)(
		[this, Discontinuous](FRHICommandListImmediate& RHICmdList)
	{
		UpdateFur_RenderThread(RHICmdList, Discontinuous);
	}
	);
}

void UGFurComponent::UpdateFur_RenderThread(FRHICommandListImmediate& RHICmdList, bool Discontinuous)
{
	FFurSceneProxy* FurProxy = (FFurSceneProxy*)SceneProxy;

	if (FurProxy)
	{
		ERHIFeatureLevel::Type SceneFeatureLevel = GetWorld()->FeatureLevel;

		int32 CurrentLOD = FurProxy->GetCurrentFurLodLevel();
		if (SkeletalGrowMesh)
		{
			const auto& LOD = SkeletalGrowMesh->GetResourceForRendering()->LODRenderData[FurProxy->GetCurrentMeshLodLevel()];
			const auto& Sections = LOD.RenderSections;
			for (int32 SectionIdx = 0; SectionIdx < Sections.Num(); SectionIdx++)
			{
				FurProxy->GetVertexFactory(SectionIdx, true)->UpdateSkeletonShaderData(ForceDistribution, MaxPhysicsOffsetLength, ReferenceToLocal, LinearOffsets, AngularOffsets, Transformations,
					Sections[SectionIdx].BoneMap, Discontinuous || CurrentLOD != LastLOD, SceneFeatureLevel);
			}
			if (!DisableMorphTargets && MasterPoseComponent.IsValid() && FurProxy->GetMorphObject(true))
			{
				int32 FurLodLevel = FurProxy->GetCurrentFurLodLevel();
				if (FurLodLevel == 0 || !LODs[FurLodLevel - 1].DisableMorphTargets)
					FurProxy->GetMorphObject(true)->Update_RenderThread(RHICmdList, MasterPoseComponent->ActiveMorphTargets, MasterPoseComponent->MorphTargetWeights, MorphRemapTables, FurProxy->GetCurrentMeshLodLevel());
			}
		}
		else if (StaticGrowMesh)
		{
			const auto& LOD = StaticGrowMesh->GetRenderData()->LODResources[FurProxy->GetCurrentMeshLodLevel()];
			const auto& Sections = LOD.Sections;
			for (int32 SectionIdx = 0; SectionIdx < Sections.Num(); SectionIdx++)
			{
				FurProxy->GetVertexFactory(SectionIdx, true)->UpdateStaticShaderData(ForceDistribution, StaticLinearOffset, StaticAngularOffset,
					StaticTransformation.GetOrigin(), Discontinuous || CurrentLOD != LastLOD, SceneFeatureLevel);
			}
		}
		LastLOD = CurrentLOD;
	}
}

void UGFurComponent::UpdateMasterBoneMap()
{
	MasterBoneMap.Empty();
	MasterBoneMap.AddDefaulted();

	if (SkeletalGrowMesh && MasterPoseComponent.IsValid() && MasterPoseComponent->SkeletalMesh)
	{
		USkeletalMesh* ParentMesh = MasterPoseComponent->SkeletalMesh;
		TArray<int32>& CurrentMasterBoneMap = MasterBoneMap[0];

		const auto& GrowMeshRefSkeleton = SkeletalGrowMesh->GetRefSkeleton();
		const auto& ParentMeshRefSkeleton = ParentMesh->GetRefSkeleton();
		CurrentMasterBoneMap.Empty(GrowMeshRefSkeleton.GetNum());
		CurrentMasterBoneMap.AddUninitialized(GrowMeshRefSkeleton.GetNum());
		if (SkeletalGrowMesh == ParentMesh)
		{
			// if the meshes are the same, the indices must match exactly so we don't need to look them up
			for (int32 i = 0; i < CurrentMasterBoneMap.Num(); i++)
			{
				CurrentMasterBoneMap[i] = i;
			}
		}
		else
		{
			for (int32 i = 0; i < CurrentMasterBoneMap.Num(); i++)
			{
				FName BoneName = GrowMeshRefSkeleton.GetBoneName(i);
				CurrentMasterBoneMap[i] = ParentMeshRefSkeleton.FindBoneIndex(BoneName);
			}
		}

		for (int32 LODIndex = 1; LODIndex < ParentMesh->GetLODNum(); LODIndex++)
		{
			MasterBoneMap.AddDefaulted();
			MasterBoneMap[LODIndex] = MasterBoneMap[0];
		}

		for (int32 LODIndex = 0; LODIndex < ParentMesh->GetLODNum(); LODIndex++)
		{
			const auto& Lod = ParentMesh->GetLODInfo(LODIndex);
			for (const auto& Bone : Lod->BonesToRemove)
			{
				int32 BoneIndex = ParentMeshRefSkeleton.FindBoneIndex(Bone.BoneName);
				if (BoneIndex >= 0)
					MasterBoneMap[LODIndex][BoneIndex] = -1;
			}
		}
	}
}

void UGFurComponent::CreateMorphRemapTable(int32 InLod)
{
	auto& MorphRemapTable = MorphRemapTables[InLod];
	if (MorphRemapTable.Num() > 0)
		return;

	auto* MasterMesh = MasterPoseComponent->SkeletalMesh->GetResourceForRendering();
	check(MasterMesh);

	const auto& MasterLodModel = MasterMesh->LODRenderData[FMath::Min(InLod, MasterMesh->LODRenderData.Num() - 1)];
	const auto& MasterPositions = MasterLodModel.StaticVertexBuffers.PositionVertexBuffer;
	const auto& MasterSkinWeights = MasterLodModel.SkinWeightVertexBuffer;
	const auto& MasterVertices = MasterLodModel.StaticVertexBuffers.StaticMeshVertexBuffer;

	auto* Mesh = SkeletalGrowMesh->GetResourceForRendering();
	const auto& LodModel = Mesh->LODRenderData[InLod];
	const auto& Positions = LodModel.StaticVertexBuffers.PositionVertexBuffer;
	const auto& SkinWeights = LodModel.SkinWeightVertexBuffer;
	const auto& Vertices = LodModel.StaticVertexBuffers.StaticMeshVertexBuffer;

	MorphRemapTable.AddUninitialized(MasterPositions.GetNumVertices());
	for (int i = 0, c = MasterPositions.GetNumVertices(); i < c; i++)
		MorphRemapTable[i] = -1;

	uint32 UVCount = FMath::Min(MasterVertices.GetNumTexCoords(), Vertices.GetNumTexCoords());

	FVector Min(FLT_MAX);
	FVector Max(-FLT_MAX);
	for (const auto& Section : LodModel.RenderSections)
	{
		for (uint32 i = Section.BaseVertexIndex; i < Section.BaseVertexIndex + Section.NumVertices; i++)
		{
			const auto& Position = Positions.VertexPosition(i);
			if (Position.X < Min.X)
				Min.X = Position.X;
			if (Position.Y < Min.Y)
				Min.Y = Position.Y;
			if (Position.Z < Min.Z)
				Min.Z = Position.Z;
			if (Position.X > Max.X)
				Max.X = Position.X;
			if (Position.Y > Max.Y)
				Max.Y = Position.Y;
			if (Position.Z > Max.Z)
				Max.Z = Position.Z;
		}
	}


	auto Hash = [&Min, &Max](const FVector& Position) {
		FVector V = (Position - Min) / (Max - Min);
		return (uint16)(V.X * (64 * 1024) + V.Y * (64 * 1024) + V.Y * (64 * 1024));
	};

	TArray<FHashTable> HashTables;
	for (const auto& Section : LodModel.RenderSections)
	{
		HashTables.Add(FHashTable(64 * 1024, Section.NumVertices));
		FHashTable& hashTable = HashTables[HashTables.Num() - 1];
		for (uint32 i = Section.BaseVertexIndex; i < Section.BaseVertexIndex + Section.NumVertices; i++)
		{
			const auto& Position = Positions.VertexPosition(i);
			hashTable.Add(Hash(Position), i);
		}
	}

	for (const auto& MasterSection : MasterLodModel.RenderSections)
	{
		for (int32 SectionIndex = 0; SectionIndex < LodModel.RenderSections.Num(); SectionIndex++)
		{
			const auto& Section = LodModel.RenderSections[SectionIndex];
			if (MasterSection.MaterialIndex == Section.MaterialIndex)
			{
				const FHashTable& hashTable = HashTables[SectionIndex];
				for (uint32 i = MasterSection.BaseVertexIndex; i < MasterSection.BaseVertexIndex + MasterSection.NumVertices; i++)
				{
					const auto& MasterPosition = MasterPositions.VertexPosition(i);
					const auto& MasterTangentX = MasterVertices.VertexTangentX(i);
					const auto& MasterTangentY = MasterVertices.VertexTangentY(i);
					const auto& MasterTangentZ = MasterVertices.VertexTangentZ(i);

					auto Compare = [&](uint32 Index) {
						const auto& Position = Positions.VertexPosition(Index);
						const auto& TangentX = Vertices.VertexTangentX(Index);
						const auto& TangentY = Vertices.VertexTangentY(Index);
						const auto& TangentZ = Vertices.VertexTangentZ(Index);
						if (MasterPosition == Position && MasterTangentX == TangentX && MasterTangentY == TangentY && MasterTangentZ == TangentZ)
						{
							for (uint32 k = 0; k < UVCount; k++)
							{
								if (MasterVertices.GetVertexUV(i, k) != Vertices.GetVertexUV(Index, k))
									return false;
							}
							MorphRemapTable[i] = Index;
							return true;
						}
						return false;
					};

					for (uint32 Idx = hashTable.First(Hash(MasterPosition)); hashTable.IsValid(Idx); Idx = hashTable.Next(Idx))
					{
						if (Compare(Idx))
							break;
					}

/*
					bool Found = false;
					for (uint32 j = FMath::Max(i, Section.BaseVertexIndex); j < Section.BaseVertexIndex + Section.NumVertices; j++)
					{
						if (Compare(j))
						{
							Found = true;
							break;
						}
					}

					if (!Found)
					{
						for (uint32 j = Section.BaseVertexIndex, e = FMath::Min(i, Section.BaseVertexIndex + Section.NumVertices); j < e; j++)
						{
							if (Compare(j))
								break;
						}
					}*/
				}
			}
		}
	}
}
