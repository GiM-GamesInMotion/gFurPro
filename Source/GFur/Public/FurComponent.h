// Copyright 2018 GiM s.r.o. All Rights Reserved.

#pragma once

#include "Runtime/Engine/Classes/Components/MeshComponent.h"
#include "FurComponent.generated.h"

USTRUCT(BlueprintType)
struct FFurLod
{
	GENERATED_USTRUCT_BODY()

	/**
	* Size of the fur on the screen at which the LOD will be used. The lower the number is, the longer it stays even if becomes smaller on the screen. Should be higher than "Min Screen Size"
	*/
	UPROPERTY(EditAnywhere, Category = "LOD")
	float ScreenSize;

	/**
	* Number of shells for this particular LOD. Should be lower than "Fur Layer Count"
	*/
	UPROPERTY(EditAnywhere, Category = "LOD", meta = (UIMin = "1", UIMax = "128", ClampMin = "1", ClampMax = "128"))
	int LayerCount;

	/**
	* Which LOD of the Grow Mesh should be used to generate fur for this LOD.
	*/
	UPROPERTY(EditAnywhere, Category = "LOD")
	int Lod;

	/**
	* If fur should react to forces and movement while using this LOD.
	*/
	UPROPERTY(EditAnywhere, Category = "LOD")
	bool PhysicsEnabled = true;

	/**
	* Turns off support for Morph Targets
	*/
	UPROPERTY(EditAnywhere, Category = "LOD")
	bool DisableMorphTargets = false;
};

/** UFurComponent */
UCLASS(editinlinenew,
	meta = (BlueprintSpawnableComponent),
	ClassGroup = Rendering,
	HideCategories = (Physics, Object, Activation, "Components|Activation"),
	meta = (DisplayName = "gFur"))
class GFUR_API UGFurComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:
	/**
	* Mesh used to generate shells. Usually this mesh will have the polys of eyes removed, interior of the mouth or claws.
	* It's necessary for the mesh to be skinned exactly like the main mesh, otherwise different artifacts could show up.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Skeletal Mesh")
	class USkeletalMesh* SkeletalGrowMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Static Mesh")
	class UStaticMesh* StaticGrowMesh;

	/**
	* Splines from 3D apps that give basic shape and length to the fur. Does not have to be used.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Guides")
	class UFurSplines* FurSplines;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Skeletal Mesh")
	TArray<class USkeletalMesh*> SkeletalGuideMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Static Mesh")
	TArray<class UStaticMesh*> StaticGuideMeshes;

	/**
	* Sets the number of shells. Less = better performance
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings", meta = (UIMin = "1", UIMax = "128", ClampMin = "1", ClampMax = "128"))
	int LayerCount;

	/**
	* If the fur becomes too small on the screen it will disappear for better performance. The lower the number the longer the fur remains on the screen even if small.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	float MinScreenSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	TArray<FFurLod> LODs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	bool LODFromParent;

	/**
	* With value 0.0 the shells are distributed linearly from root to tip. With values larger than 0.0, distribution becomes nonlinear,
	* pushing the shells more to the tip where the shells tend to be more visible if the layer count is relatively low.
	* With low shell count, using bias > 0 can help reduce artefacts at the fur tip.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	float ShellBias;

	/**
	* Sets the length of the fur if "fur splines" are not used. If "fur splines" are used, this value scales them (1.0 = no change, >1.0 splines are scaled up, <1.0 splines are scaled down)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	float FurLength;

	/**
	* Overrides length of too short splines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings", meta = (UIMin = "0.001", ClampMin = "0.001"))
	float MinFurLength;

	/**
	* Removes faces without splines.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	bool RemoveFacesWithoutSplines;

	/**
	* If fur should react to forces and movement.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	bool PhysicsEnabled;

	/**
	* With value = 1.0, the forces affecting fur are distributed linearly from root to tip.
	* Values above 1.0 push the forces more to the tip, leaving the lower parts of fur strands less affected.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float ForceDistribution;

	/**
	* Higher values make the fur bend less under the different forces
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float Stiffness;

	/**
	* How fast the fur loses energy from different forces and comes to stop.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float Damping;

	/**
	* Constant force affecting the fur. Usually this emulates gravity.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	FVector ConstantForce;

	/**
	* Maximum allowed forces to act on the fur. This allows you to clamp the motion of the fur if it's too much, but otherwise the fur looks good.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float MaxForce;

	/**
	* Clamps the forces coming from rotational movement of the bones. This value is multiplying the "Max Force" parameter.
	* At value 1.0 torque forces are effectively clamped with "Max Force", values below 1.0 clamp the torque forces even more than "Max Force",
	* values above 1.0 give more room to the torque forces that would be otherwise clamped if "Max Force Torque Factor" would be 1.0
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float MaxForceTorqueFactor;

	/**
	* Length of the reference hair for which physics is calculated for. 1 takes longer hair into account, 0 takes more of the short hair.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float ReferenceHairBias;

	/**
	* With value = 0.0, all the fur hair will bend closer to "Reference Hair". Values closer to 1.0 will bend longer and shorter hair the same way.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float HairLengthForceUniformity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Physics")
	float MaxPhysicsOffsetLength;

	/**
	* Introduces noise to the shell vertices along the normal. This helps to break up the uniformity of the shell slices when viewed from the side.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	float NoiseStrength;

	/**
	* Turns off support for Morph Targets
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "gFur Shell settings")
	bool DisableMorphTargets;

	/**
	 * Allows adjusting the desired streaming distance of streaming textures that uses UV 0.
	 * 1.0 is the default, whereas a higher value makes the textures stream in sooner from far away.
	 * A lower value (0.0-1.0) makes the textures stream in later (you have to be closer).
	 * Value can be < 0 (from legcay content, or code changes)
	 */
	UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category=SkeletalMesh)
	float StreamingDistanceMultiplier;

	UFUNCTION(BlueprintCallable, Category = "gFur Shell settings")
	void RegenerateFur();

	const TArray<int32>& GetFurSplineMap() const;
	const TArray<FVector>& GetVertexNormals() const;

public:
	// Begin UPrimitiveComponent interface.
	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
	virtual int32 GetMaterialIndex(FName MaterialSlotName) const override;
	virtual TArray<FName> GetMaterialSlotNames() const override;
	virtual bool IsMaterialSlotNameValid(FName MaterialSlotName) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	virtual void SetMaterialByName(FName MaterialSlotName, class UMaterialInterface* Material) override;
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;
	virtual bool GetMaterialStreamingData(int32 MaterialIndex, FPrimitiveMaterialInfo& MaterialData) const override;
	virtual void GetStreamingRenderAssetInfo(FStreamingTextureLevelContext& LevelContext, TArray<FStreamingRenderAssetPrimitiveInfo>& OutStreamingRenderAssets) const override;
	virtual int32 GetNumMaterials() const override;

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual class UBodySetup* GetBodySetup() override;
	// End UPrimitiveComponent interface.

	const TArray<class UMaterialInstanceDynamic*>& GetFurMaterials() const { return FurMaterials; }

	TWeakObjectPtr< class USkinnedMeshComponent > GetMasterPoseComponent() const { return MasterPoseComponent; }

protected:
	//~ Begin UActorComponent Interface
	virtual void CreateRenderState_Concurrent(FRegisterComponentContext* Context) override;
	virtual void SendRenderDynamicData_Concurrent() override;
	virtual void DestroyRenderState_Concurrent() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//~ End UActorComponent Interface

private:
	TWeakObjectPtr< class USkinnedMeshComponent > MasterPoseComponent;
	TArray<TArray<int32>> MasterBoneMap;
	TArray<FMatrix> ReferenceToLocal;
	TArray<FMatrix> Transformations;
	TArray<FVector> LinearVelocities;
	TArray<FVector> AngularVelocities;
	TArray<FVector> LinearOffsets;
	TArray<FVector> AngularOffsets;
	TArray< class UMaterialInstanceDynamic* > FurMaterials;
	TArray< class FFurData* > FurData;
	TArray< TArray< int32 > > MorphRemapTables;

	FVector StaticLinearOffset;
	FVector StaticAngularOffset;
	FVector StaticLinearVelocity;
	FVector StaticAngularVelocity;
	FMatrix StaticTransformation;
	bool OldPositionValid = false;
	int32 LastLOD = -1;

	float LastDeltaTime;

	uint32 LastRevisionNumber = 0;

	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	// Begin USceneComponent interface.

	void updateFur();
	void UpdateFur_RenderThread(FRHICommandListImmediate& RHICmdList, bool Discontinuous);
	void UpdateMasterBoneMap();
	void CreateMorphRemapTable(int32 InLod);
};
