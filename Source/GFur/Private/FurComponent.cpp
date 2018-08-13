// Copyright 2018 GiM s.r.o. All Rights Reserved.

#include "FurComponent.h"
#include "GFur.h"
#include "FurSplines.h"
#include "FurData.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"
#include "Runtime/Engine/Public/DynamicMeshBuilder.h"
#include "Runtime/Engine/Public/GPUSkinVertexFactory.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Runtime/Engine/Classes/Components/SkinnedMeshComponent.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Runtime/Launch/Resources/Version.h"
#include "ShaderParameterUtils.h"
#include "FurSkinData.h"
#include "FurStaticData.h"


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
			d->CreateVertexFactories(VertexFactories, InFeatureLevel);
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
							UMaterialInstanceDynamic* material = FurMaterials[section.MaterialIndex];
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
						BatchElement.MinVertexIndex = section.MinVertexIndex;
						BatchElement.MaxVertexIndex = section.MaxVertexIndex;
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

UMaterialInterface* UGFurComponent::GetMaterial(int32 MaterialIndex) const
{
	if (MaterialIndex < OverrideMaterials.Num() && OverrideMaterials[MaterialIndex])
	{
		return OverrideMaterials[MaterialIndex];
	}
	else if (SkeletalGrowMesh && MaterialIndex < SkeletalGrowMesh->Materials.Num() && SkeletalGrowMesh->Materials[MaterialIndex].MaterialInterface)
	{
		return SkeletalGrowMesh->Materials[MaterialIndex].MaterialInterface;
	}
	else if (StaticGrowMesh && MaterialIndex < StaticGrowMesh->StaticMaterials.Num() && StaticGrowMesh->StaticMaterials[MaterialIndex].MaterialInterface)
	{
		return StaticGrowMesh->StaticMaterials[MaterialIndex].MaterialInterface;
	}

	return NULL;
}

int32 UGFurComponent::GetMaterialIndex(FName MaterialSlotName) const
{
	if (SkeletalGrowMesh)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < SkeletalGrowMesh->Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = SkeletalGrowMesh->Materials[MaterialIndex];
			if (SkeletalMaterial.MaterialSlotName == MaterialSlotName)
			{
				return MaterialIndex;
			}
		}
	}
	else if (StaticGrowMesh)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < StaticGrowMesh->StaticMaterials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = StaticGrowMesh->StaticMaterials[MaterialIndex];
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
		for (int32 MaterialIndex = 0; MaterialIndex < SkeletalGrowMesh->Materials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = SkeletalGrowMesh->Materials[MaterialIndex];
			MaterialNames.Add(SkeletalMaterial.MaterialSlotName);
		}
	}
	else if (StaticGrowMesh)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < StaticGrowMesh->StaticMaterials.Num(); ++MaterialIndex)
		{
			const auto& SkeletalMaterial = StaticGrowMesh->StaticMaterials[MaterialIndex];
			MaterialNames.Add(SkeletalMaterial.MaterialSlotName);
		}
	}
	return MaterialNames;
}

bool UGFurComponent::IsMaterialSlotNameValid(FName MaterialSlotName) const
{
	return GetMaterialIndex(MaterialSlotName) >= 0;
}

void UGFurComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const
{
	if (SkeletalGrowMesh)
	{
		// The max number of materials used is the max of the materials on the skeletal mesh and the materials on the mesh component
		const int32 NumMaterials = FMath::Max(SkeletalGrowMesh->Materials.Num(), OverrideMaterials.Num());
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
		// The max number of materials used is the max of the materials on the skeletal mesh and the materials on the mesh component
		const int32 NumMaterials = FMath::Max(StaticGrowMesh->StaticMaterials.Num(), OverrideMaterials.Num());
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

void UGFurComponent::GetStreamingTextureInfo(FStreamingTextureLevelContext& LevelContext, TArray<FStreamingTexturePrimitiveInfo>& OutStreamingTextures) const
{
	GetStreamingTextureInfoInner(LevelContext, nullptr, GetComponentTransform().GetMaximumAxisScale(), OutStreamingTextures);
}

int32 UGFurComponent::GetNumMaterials() const
{
	if (SkeletalGrowMesh)
	{
		return SkeletalGrowMesh->Materials.Num();
	}
	else if (StaticGrowMesh)
	{
		return StaticGrowMesh->StaticMaterials.Num();
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

		if (SkeletalGrowMesh && SkeletalGrowMesh->GetResourceForRendering())
		{
			UpdateMasterBoneMap();
			TArray<FFurData*> Array;
			Array.Add(FFurSkinData::CreateFurData(LayerCount, 0, this));
			for (FFurLod& lod : LODs)
				Array.Add(FFurSkinData::CreateFurData(lod.LayerCount, FMath::Min(SkeletalGrowMesh->GetImportedResource()->LODModels.Num() - 1, lod.Lod), this));

			FurData = Array;
			return new FFurSceneProxy(this, FurData, LODs, FurMaterials, CastShadow, GetWorld()->FeatureLevel);
		}
		else if (StaticGrowMesh && StaticGrowMesh->RenderData)
		{
			TArray<FFurData*> Array;
			Array.Add(FFurStaticData::CreateFurData(LayerCount, 0, this));
			for (FFurLod& lod : LODs)
				Array.Add(FFurStaticData::CreateFurData(lod.LayerCount, FMath::Min(StaticGrowMesh->RenderData->LODResources.Num() - 1, lod.Lod), this));

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
		FBoxSphereBounds Bounds = StaticGrowMesh->GetBounds();
		Bounds.ExpandBy(FMath::Max(FurLength, 0.001f));
		return Bounds.TransformBy(LocalToWorld);
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

	float DeltaTime = fminf(LastDeltaTime, 1.0f);
	float ReferenceFurLength = FMath::Max(0.00001f, Scene->GetFurData()->CurrentMaxFurLength * ReferenceHairBias + Scene->GetFurData()->CurrentMinFurLength * (1.0f - ReferenceHairBias));
//	float ForceFactor = 1.0f / (powf(ReferenceFurLength, FurForcePower) * fmaxf(FurStiffness, 0.000001f));
	float ForceFactor = 1.0f / powf(ReferenceFurLength, ForceDistribution);
	float DampingClamped = fmaxf(Damping, 0.000001f);
	float DampingFactor = powf(1.0f - (DampingClamped / (DampingClamped + 1.0f)), DeltaTime);
	float MaxForceFinal = (MaxForce * ReferenceFurLength) / powf(ReferenceFurLength, ForceDistribution);
	float MaxTorque = MaxForceTorqueFactor * MaxForceFinal / Scene->GetFurData()->MaxVertexBoneDistance;
//	FVector FurForceFinal = FurForce * (fmaxf(FurWeight, 0.000001f) * ForceFactor);//TODO
	FVector FurForceFinal = ConstantForce * ReferenceFurLength * ForceFactor / Stiffness;//TODO

	float x = DeltaTime * Stiffness;

	FMatrix ToWorld = GetComponentTransform().ToMatrixNoScale();

	auto Physics = [&](const FMatrix& NewTransformation, FMatrix& Transformations, FVector& LinearOffset, FVector& LinearVelocity
		, FVector& AngularOffset, FVector& AngularVelocity) {
		FVector d = (NewTransformation.GetOrigin() - Transformations.GetOrigin());
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

		FQuat rdiff = NewTransformation.ToQuat() * Transformations.ToQuat().Inverse();
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

		Transformations = NewTransformation;
	};

	if (SkeletalGrowMesh)
	{
		const USkeletalMesh* const ThisMesh = SkeletalGrowMesh;
		const USkinnedMeshComponent* const MasterComp = MasterPoseComponent.Get();
		const USkeletalMesh* const MasterCompMesh = MasterComp ? MasterComp->SkeletalMesh : nullptr;
		const auto& LOD = SkeletalGrowMesh->GetImportedResource()->LODModels[Scene->GetCurrentLodLevel()];

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
			for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ++ThisBoneIndex)
			{
				FMatrix NewTransformation;
				if (ValidTempMatrices[ThisBoneIndex])
				{
					ReferenceToLocal[ThisBoneIndex] = ThisMesh->RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
					NewTransformation = TempMatrices[ThisBoneIndex] * ToWorld;
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
					ReferenceToLocal[ThisBoneIndex] = ThisMesh->RefBasesInvMatrix[ThisBoneIndex] * TempMatrices[ThisBoneIndex];
					Transformations[ThisBoneIndex] = TempMatrices[ThisBoneIndex] * ToWorld;
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
	}
	else
	{
		check(StaticGrowMesh);
		if (StaticOldPositionValid)
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

			StaticOldPositionValid = true;
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
		ERHIFeatureLevel::Type SceneFeatureLevel = GetWorld()->FeatureLevel;

		if (SkeletalGrowMesh)
		{
			const auto& LOD = SkeletalGrowMesh->GetImportedResource()->LODModels[FurProxy->GetCurrentLodLevel()];
			const auto& Sections = LOD.Sections;
			for (int32 SectionIdx = 0; SectionIdx < Sections.Num(); SectionIdx++)
			{
				FurProxy->GetVertexFactory(SectionIdx)->UpdateSkeletonShaderData(ForceDistribution, ReferenceToLocal, LinearOffsets, AngularOffsets, Transformations,
					Sections[SectionIdx].BoneMap, FrameNumberToPrepare, SceneFeatureLevel);
			}
		}
		else if (StaticGrowMesh)
		{
			const auto& LOD = StaticGrowMesh->RenderData->LODResources[FurProxy->GetCurrentLodLevel()];
			const auto& Sections = LOD.Sections;
			for (int32 SectionIdx = 0; SectionIdx < Sections.Num(); SectionIdx++)
			{
				TArray<FBoneIndexType> BoneMap;
				FurProxy->GetVertexFactory(SectionIdx)->UpdateStaticShaderData(ForceDistribution, StaticLinearOffset, StaticAngularOffset,
					StaticTransformation.GetOrigin(), FrameNumberToPrepare, SceneFeatureLevel);
			}
		}
	}
}

void UGFurComponent::UpdateMasterBoneMap()
{
	MasterBoneMap.Empty();

	if (SkeletalGrowMesh && MasterPoseComponent.IsValid() && MasterPoseComponent->SkeletalMesh)
	{
		USkeletalMesh* ParentMesh = MasterPoseComponent->SkeletalMesh;

		MasterBoneMap.Empty(SkeletalGrowMesh->RefSkeleton.GetNum());
		MasterBoneMap.AddUninitialized(SkeletalGrowMesh->RefSkeleton.GetNum());
		if (SkeletalGrowMesh == ParentMesh)
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
				FName BoneName = SkeletalGrowMesh->RefSkeleton.GetBoneName(i);
				MasterBoneMap[i] = ParentMesh->RefSkeleton.FindBoneIndex(BoneName);
			}
		}
	}
}
