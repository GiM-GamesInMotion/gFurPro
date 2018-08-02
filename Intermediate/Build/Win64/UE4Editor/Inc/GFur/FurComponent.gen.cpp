// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/FurComponent.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurComponent() {}
// Cross Module References
	GFUR_API UScriptStruct* Z_Construct_UScriptStruct_FFurLod();
	UPackage* Z_Construct_UPackage__Script_GFur();
	GFUR_API UClass* Z_Construct_UClass_UGFurComponent_NoRegister();
	GFUR_API UClass* Z_Construct_UClass_UGFurComponent();
	ENGINE_API UClass* Z_Construct_UClass_UMeshComponent();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
	ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USkeletalMesh_NoRegister();
	GFUR_API UClass* Z_Construct_UClass_UFurSplines_NoRegister();
// End Cross Module References
class UScriptStruct* FFurLod::StaticStruct()
{
	static class UScriptStruct* Singleton = NULL;
	if (!Singleton)
	{
		extern GFUR_API uint32 Get_Z_Construct_UScriptStruct_FFurLod_CRC();
		Singleton = GetStaticStruct(Z_Construct_UScriptStruct_FFurLod, Z_Construct_UPackage__Script_GFur(), TEXT("FurLod"), sizeof(FFurLod), Get_Z_Construct_UScriptStruct_FFurLod_CRC());
	}
	return Singleton;
}
static FCompiledInDeferStruct Z_CompiledInDeferStruct_UScriptStruct_FFurLod(FFurLod::StaticStruct, TEXT("/Script/GFur"), TEXT("FurLod"), false, nullptr, nullptr);
static struct FScriptStruct_GFur_StaticRegisterNativesFFurLod
{
	FScriptStruct_GFur_StaticRegisterNativesFFurLod()
	{
		UScriptStruct::DeferCppStructOps(FName(TEXT("FurLod")),new UScriptStruct::TCppStructOps<FFurLod>);
	}
} ScriptStruct_GFur_StaticRegisterNativesFFurLod;
	UScriptStruct* Z_Construct_UScriptStruct_FFurLod()
	{
#if WITH_HOT_RELOAD
		extern uint32 Get_Z_Construct_UScriptStruct_FFurLod_CRC();
		UPackage* Outer = Z_Construct_UPackage__Script_GFur();
		static UScriptStruct* ReturnStruct = FindExistingStructIfHotReloadOrDynamic(Outer, TEXT("FurLod"), sizeof(FFurLod), Get_Z_Construct_UScriptStruct_FFurLod_CRC(), false);
#else
		static UScriptStruct* ReturnStruct = nullptr;
#endif
		if (!ReturnStruct)
		{
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
				{ "BlueprintType", "true" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			auto NewStructOpsLambda = []() -> void* { return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FFurLod>(); };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Lod_MetaData[] = {
				{ "Category", "LOD" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Lod = { UE4CodeGen_Private::EPropertyClass::Int, "Lod", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, Lod), METADATA_PARAMS(NewProp_Lod_MetaData, ARRAY_COUNT(NewProp_Lod_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[] = {
				{ "Category", "LOD" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Number of shells for this particular LOD. Should be lower than \"Fur Layer Count\"" },
			};
#endif
			static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount = { UE4CodeGen_Private::EPropertyClass::Int, "LayerCount", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, LayerCount), METADATA_PARAMS(NewProp_LayerCount_MetaData, ARRAY_COUNT(NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ScreenSize_MetaData[] = {
				{ "Category", "LOD" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Size of the fur on the screen at which the LOD will be used. The lower the number is, the longer it stays even if becomes smaller on the screen. Should be higher than \"Min Screen Size\"" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ScreenSize = { UE4CodeGen_Private::EPropertyClass::Float, "ScreenSize", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, ScreenSize), METADATA_PARAMS(NewProp_ScreenSize_MetaData, ARRAY_COUNT(NewProp_ScreenSize_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Lod,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_LayerCount,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ScreenSize,
			};
			static const UE4CodeGen_Private::FStructParams ReturnStructParams = {
				(UObject* (*)())Z_Construct_UPackage__Script_GFur,
				nullptr,
				&UE4CodeGen_Private::TNewCppStructOpsWrapper<decltype(NewStructOpsLambda)>::NewCppStructOps,
				"FurLod",
				RF_Public|RF_Transient|RF_MarkAsNative,
				EStructFlags(0x00000001),
				sizeof(FFurLod),
				alignof(FFurLod),
				PropPointers, ARRAY_COUNT(PropPointers),
				METADATA_PARAMS(Struct_MetaDataParams, ARRAY_COUNT(Struct_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FFurLod_CRC() { return 2075799263U; }
	void UGFurComponent::StaticRegisterNativesUGFurComponent()
	{
	}
	UClass* Z_Construct_UClass_UGFurComponent_NoRegister()
	{
		return UGFurComponent::StaticClass();
	}
	UClass* Z_Construct_UClass_UGFurComponent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UMeshComponent,
				(UObject* (*)())Z_Construct_UPackage__Script_GFur,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "BlueprintSpawnableComponent", "" },
				{ "ClassGroupNames", "Rendering" },
				{ "DisplayName", "gFur" },
				{ "HideCategories", "Collision Physics Object Activation Components|Activation Mobility Trigger" },
				{ "IncludePath", "FurComponent.h" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "UFurComponent" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NoiseStrength_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Introduces noise to the shell vertices along the normal. This helps to break up the uniformity of the shell slices when viewed from the side." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_NoiseStrength = { UE4CodeGen_Private::EPropertyClass::Float, "NoiseStrength", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, NoiseStrength), METADATA_PARAMS(NewProp_NoiseStrength_MetaData, ARRAY_COUNT(NewProp_NoiseStrength_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_HairLengthForceUniformity_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "With value = 0.0, all the fur hair will bend closer to \"Reference Hair\". Values closer to 1.0 will bend longer and shorter hair the same way." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_HairLengthForceUniformity = { UE4CodeGen_Private::EPropertyClass::Float, "HairLengthForceUniformity", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, HairLengthForceUniformity), METADATA_PARAMS(NewProp_HairLengthForceUniformity_MetaData, ARRAY_COUNT(NewProp_HairLengthForceUniformity_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReferenceHairBias_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Length of the reference hair for which physics is calculated for. 1 takes longer hair into account, 0 takes more of the short hair." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReferenceHairBias = { UE4CodeGen_Private::EPropertyClass::Float, "ReferenceHairBias", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ReferenceHairBias), METADATA_PARAMS(NewProp_ReferenceHairBias_MetaData, ARRAY_COUNT(NewProp_ReferenceHairBias_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxForceTorqueFactor_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Clamps the forces coming from rotational movement of the bones. This value is multiplying the \"Max Force\" parameter.\nAt value 1.0 torque forces are effectively clamped with \"Max Force\", values below 1.0 clamp the torque forces even more than \"Max Force\",\nvalues above 1.0 give more room to the torque forces that would be otherwise clamped if \"Max Force Torque Factor\" would be 1.0" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxForceTorqueFactor = { UE4CodeGen_Private::EPropertyClass::Float, "MaxForceTorqueFactor", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForceTorqueFactor), METADATA_PARAMS(NewProp_MaxForceTorqueFactor_MetaData, ARRAY_COUNT(NewProp_MaxForceTorqueFactor_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxForce_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Maximum allowed forces to act on the fur. This allows you to clamp the motion of the fur if it's too much, but otherwise the fur looks good." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxForce = { UE4CodeGen_Private::EPropertyClass::Float, "MaxForce", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForce), METADATA_PARAMS(NewProp_MaxForce_MetaData, ARRAY_COUNT(NewProp_MaxForce_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ConstantForce_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Constant force affecting the fur. Usually this emulates gravity." },
			};
#endif
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_ConstantForce = { UE4CodeGen_Private::EPropertyClass::Struct, "ConstantForce", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ConstantForce), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(NewProp_ConstantForce_MetaData, ARRAY_COUNT(NewProp_ConstantForce_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Damping_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "How fast the fur loses energy from different forces and comes to stop." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Damping = { UE4CodeGen_Private::EPropertyClass::Float, "Damping", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, Damping), METADATA_PARAMS(NewProp_Damping_MetaData, ARRAY_COUNT(NewProp_Damping_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Stiffness_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Higher values make the fur bend less under the different forces" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Stiffness = { UE4CodeGen_Private::EPropertyClass::Float, "Stiffness", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, Stiffness), METADATA_PARAMS(NewProp_Stiffness_MetaData, ARRAY_COUNT(NewProp_Stiffness_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ForceDistribution_MetaData[] = {
				{ "Category", "gFur Physics" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "With value = 1.0, the forces affecting fur are distributed linearly from root to tip.\nValues above 1.0 push the forces more to the tip, leaving the lower parts of fur strands less affected." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ForceDistribution = { UE4CodeGen_Private::EPropertyClass::Float, "ForceDistribution", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ForceDistribution), METADATA_PARAMS(NewProp_ForceDistribution_MetaData, ARRAY_COUNT(NewProp_ForceDistribution_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MinFurLength_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Overrides length of too short splines." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MinFurLength = { UE4CodeGen_Private::EPropertyClass::Float, "MinFurLength", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MinFurLength), METADATA_PARAMS(NewProp_MinFurLength_MetaData, ARRAY_COUNT(NewProp_MinFurLength_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FurLength_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Sets the length of the fur if \"fur splines\" are not used. If \"fur splines\" are used, this value scales them (1.0 = no change, >1.0 splines are scaled up, <1.0 splines are scaled down)" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_FurLength = { UE4CodeGen_Private::EPropertyClass::Float, "FurLength", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, FurLength), METADATA_PARAMS(NewProp_FurLength_MetaData, ARRAY_COUNT(NewProp_FurLength_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ShellBias_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "With value 0.0 the shells are distributed linearly from root to tip. With values larger than 0.0, distribution becomes nonlinear,\npushing the shells more to the tip where the shells tend to be more visible if the layer count is relatively low.\nWith low shell count, using bias > 0 can help reduce artefacts at the fur tip." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ShellBias = { UE4CodeGen_Private::EPropertyClass::Float, "ShellBias", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ShellBias), METADATA_PARAMS(NewProp_ShellBias_MetaData, ARRAY_COUNT(NewProp_ShellBias_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LODs_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_LODs = { UE4CodeGen_Private::EPropertyClass::Array, "LODs", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, LODs), METADATA_PARAMS(NewProp_LODs_MetaData, ARRAY_COUNT(NewProp_LODs_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_LODs_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "LODs", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FFurLod, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MinScreenSize_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "If the fur becomes too small on the screen it will disappear for better performance. The lower the number the longer the fur remains on the screen even if small." },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MinScreenSize = { UE4CodeGen_Private::EPropertyClass::Float, "MinScreenSize", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MinScreenSize), METADATA_PARAMS(NewProp_MinScreenSize_MetaData, ARRAY_COUNT(NewProp_MinScreenSize_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[] = {
				{ "Category", "gFur Shell settings" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Sets the number of shells. Less = better performance" },
			};
#endif
			static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount = { UE4CodeGen_Private::EPropertyClass::Int, "LayerCount", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, LayerCount), METADATA_PARAMS(NewProp_LayerCount_MetaData, ARRAY_COUNT(NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticGuideMeshes_MetaData[] = {
				{ "Category", "gFur Static Mesh" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_StaticGuideMeshes = { UE4CodeGen_Private::EPropertyClass::Array, "StaticGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGuideMeshes), METADATA_PARAMS(NewProp_StaticGuideMeshes_MetaData, ARRAY_COUNT(NewProp_StaticGuideMeshes_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticGuideMeshes_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "StaticGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SkeletalGuideMeshes_MetaData[] = {
				{ "Category", "gFur Skeletal Mesh" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_SkeletalGuideMeshes = { UE4CodeGen_Private::EPropertyClass::Array, "SkeletalGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGuideMeshes), METADATA_PARAMS(NewProp_SkeletalGuideMeshes_MetaData, ARRAY_COUNT(NewProp_SkeletalGuideMeshes_MetaData)) };
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SkeletalGuideMeshes_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "SkeletalGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FurSplines_MetaData[] = {
				{ "Category", "gFur Mesh" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Splines from 3D apps that give basic shape and length to the fur. Does not have to be used." },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_FurSplines = { UE4CodeGen_Private::EPropertyClass::Object, "FurSplines", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, FurSplines), Z_Construct_UClass_UFurSplines_NoRegister, METADATA_PARAMS(NewProp_FurSplines_MetaData, ARRAY_COUNT(NewProp_FurSplines_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticGrowMesh_MetaData[] = {
				{ "Category", "gFur Static Mesh" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticGrowMesh = { UE4CodeGen_Private::EPropertyClass::Object, "StaticGrowMesh", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGrowMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(NewProp_StaticGrowMesh_MetaData, ARRAY_COUNT(NewProp_StaticGrowMesh_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SkeletalGrowMesh_MetaData[] = {
				{ "Category", "gFur Skeletal Mesh" },
				{ "ModuleRelativePath", "Private/FurComponent.h" },
				{ "ToolTip", "Mesh used to generate shells. Usually this mesh will have the polys of eyes removed, interior of the mouth or claws.\nIt's necessary for the mesh to be skinned exactly like the main mesh, otherwise different artifacts could show up." },
			};
#endif
			static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SkeletalGrowMesh = { UE4CodeGen_Private::EPropertyClass::Object, "SkeletalGrowMesh", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGrowMesh), Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(NewProp_SkeletalGrowMesh_MetaData, ARRAY_COUNT(NewProp_SkeletalGrowMesh_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_NoiseStrength,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_HairLengthForceUniformity,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ReferenceHairBias,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MaxForceTorqueFactor,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MaxForce,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ConstantForce,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Damping,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Stiffness,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ForceDistribution,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MinFurLength,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_FurLength,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ShellBias,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_LODs,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_LODs_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_MinScreenSize,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_LayerCount,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_StaticGuideMeshes,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_StaticGuideMeshes_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_SkeletalGuideMeshes,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_SkeletalGuideMeshes_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_FurSplines,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_StaticGrowMesh,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_SkeletalGrowMesh,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UGFurComponent>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UGFurComponent::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00B01080u,
				nullptr, 0,
				PropPointers, ARRAY_COUNT(PropPointers),
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGFurComponent, 3788916711);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGFurComponent(Z_Construct_UClass_UGFurComponent, &UGFurComponent::StaticClass, TEXT("/Script/GFur"), TEXT("UGFurComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGFurComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
