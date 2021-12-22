// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "FurData.h"

/** Fur Static Data */
class FFurStaticData: public FFurData
{
public:
	static FFurStaticData* CreateFurData(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent);
	static void DestroyFurData(const TArray<FFurData*>& InFurDataArray);

	virtual void CreateVertexFactories(TArray<FFurVertexFactory*>& VertexFactories, FVertexBuffer* InMorphVertexBuffer, bool InPhysics, ERHIFeatureLevel::Type InFeatureLevel) override;
protected:
	UStaticMesh* StaticMesh = nullptr;
	TArray<UStaticMesh*> GuideMeshes;

#if WITH_EDITORONLY_DATA
	FDelegateHandle FurSplinesChangeHandle;
	FDelegateHandle FurSplinesCombHandle;
	FDelegateHandle StaticMeshChangeHandle;
	TArray<FDelegateHandle> GuideMeshesChangeHandles;
#endif // WITH_EDITORONLY_DATA

	FFurStaticData() {}
	~FFurStaticData();

	void UnbindChangeDelegates();
	void Set(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent);

	bool Compare(int32 InFurLayerCount, int32 InLod, class UGFurComponent* InFurComponent);
	bool Similar(int32 InLod, class UGFurComponent* InFurComponent);

	void BuildFur(BuildType Build);

	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	void BuildFur(const FStaticMeshLODResources& LodRenderData, BuildType Build);
	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
	void BuildFur(const FStaticMeshLODResources& LodRenderData, BuildType Build);

	void BuildFur(const TArray<uint32>& InVertexSet);
	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT>
	void BuildFur(const FStaticMeshLODResources& LodRenderData, const TArray<uint32>& InVertexSet);
	template<EStaticMeshVertexTangentBasisType TangentBasisTypeT, EStaticMeshVertexUVType UVTypeT>
	void BuildFur(const TArray<uint32>& InVertexSet);
};

/** Generate Splines */
void GenerateSplines(UFurSplines* Splines, UStaticMesh* InStaticMesh, int32 InLod, const TArray<UStaticMesh*>& InGuideMeshes);
