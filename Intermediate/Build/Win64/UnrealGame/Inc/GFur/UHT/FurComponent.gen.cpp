// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "FurComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurComponent() {}
// Cross Module References
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
	ENGINE_API UClass* Z_Construct_UClass_UMeshComponent();
	ENGINE_API UClass* Z_Construct_UClass_USkeletalMesh_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UStaticMesh_NoRegister();
	GFUR_API UClass* Z_Construct_UClass_UFurSplines_NoRegister();
	GFUR_API UClass* Z_Construct_UClass_UGFurComponent();
	GFUR_API UClass* Z_Construct_UClass_UGFurComponent_NoRegister();
	GFUR_API UScriptStruct* Z_Construct_UScriptStruct_FFurLod();
	UPackage* Z_Construct_UPackage__Script_GFur();
// End Cross Module References
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_FurLod;
class UScriptStruct* FFurLod::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_FurLod.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_FurLod.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FFurLod, (UObject*)Z_Construct_UPackage__Script_GFur(), TEXT("FurLod"));
	}
	return Z_Registration_Info_UScriptStruct_FurLod.OuterSingleton;
}
template<> GFUR_API UScriptStruct* StaticStruct<FFurLod>()
{
	return FFurLod::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FFurLod_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ScreenSize_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ScreenSize;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Lod_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_Lod;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_PhysicsEnabled_MetaData[];
#endif
		static void NewProp_PhysicsEnabled_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_PhysicsEnabled;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DisableMorphTargets_MetaData[];
#endif
		static void NewProp_DisableMorphTargets_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_DisableMorphTargets;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FFurLod_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FFurLod>();
	}
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData[] = {
		{ "Category", "LOD" },
		{ "Comment", "/**\n\x09* Size of the fur on the screen at which the LOD will be used. The lower the number is, the longer it stays even if becomes smaller on the screen. Should be higher than \"Min Screen Size\"\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Size of the fur on the screen at which the LOD will be used. The lower the number is, the longer it stays even if becomes smaller on the screen. Should be higher than \"Min Screen Size\"" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize = { "ScreenSize", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FFurLod, ScreenSize), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData[] = {
		{ "Category", "LOD" },
		{ "ClampMax", "128" },
		{ "ClampMin", "1" },
		{ "Comment", "/**\n\x09* Number of shells for this particular LOD. Should be lower than \"Fur Layer Count\"\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Number of shells for this particular LOD. Should be lower than \"Fur Layer Count\"" },
		{ "UIMax", "128" },
		{ "UIMin", "1" },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount = { "LayerCount", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FFurLod, LayerCount), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData[] = {
		{ "Category", "LOD" },
		{ "Comment", "/**\n\x09* Which LOD of the Grow Mesh should be used to generate fur for this LOD.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Which LOD of the Grow Mesh should be used to generate fur for this LOD." },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod = { "Lod", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FFurLod, Lod), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData[] = {
		{ "Category", "LOD" },
		{ "Comment", "/**\n\x09* If fur should react to forces and movement while using this LOD.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If fur should react to forces and movement while using this LOD." },
	};
#endif
	void Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_SetBit(void* Obj)
	{
		((FFurLod*)Obj)->PhysicsEnabled = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled = { "PhysicsEnabled", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(FFurLod), &Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets_MetaData[] = {
		{ "Category", "LOD" },
		{ "Comment", "/**\n\x09* Turns off support for Morph Targets\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Turns off support for Morph Targets" },
	};
#endif
	void Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets_SetBit(void* Obj)
	{
		((FFurLod*)Obj)->DisableMorphTargets = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets = { "DisableMorphTargets", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(FFurLod), &Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_DisableMorphTargets,
	};
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FFurLod_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
		nullptr,
		&NewStructOps,
		"FurLod",
		sizeof(FFurLod),
		alignof(FFurLod),
		Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FFurLod()
	{
		if (!Z_Registration_Info_UScriptStruct_FurLod.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_FurLod.InnerSingleton, Z_Construct_UScriptStruct_FFurLod_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_FurLod.InnerSingleton;
	}
	DEFINE_FUNCTION(UGFurComponent::execRegenerateFur)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->RegenerateFur();
		P_NATIVE_END;
	}
	void UGFurComponent::StaticRegisterNativesUGFurComponent()
	{
		UClass* Class = UGFurComponent::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "RegenerateFur", &UGFurComponent::execRegenerateFur },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics::Function_MetaDataParams[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UGFurComponent, nullptr, "RegenerateFur", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UGFurComponent_RegenerateFur()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UGFurComponent_RegenerateFur_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UGFurComponent);
	UClass* Z_Construct_UClass_UGFurComponent_NoRegister()
	{
		return UGFurComponent::StaticClass();
	}
	struct Z_Construct_UClass_UGFurComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_SkeletalGrowMesh_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_SkeletalGrowMesh;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_StaticGrowMesh_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_StaticGrowMesh;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FurSplines_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_FurSplines;
		static const UECodeGen_Private::FObjectPropertyParams NewProp_SkeletalGuideMeshes_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_SkeletalGuideMeshes_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_SkeletalGuideMeshes;
		static const UECodeGen_Private::FObjectPropertyParams NewProp_StaticGuideMeshes_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_StaticGuideMeshes_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_StaticGuideMeshes;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MinScreenSize_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MinScreenSize;
		static const UECodeGen_Private::FStructPropertyParams NewProp_LODs_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LODs_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_LODs;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LODFromParent_MetaData[];
#endif
		static void NewProp_LODFromParent_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_LODFromParent;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ShellBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ShellBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FurLength_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_FurLength;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MinFurLength_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MinFurLength;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_RemoveFacesWithoutSplines_MetaData[];
#endif
		static void NewProp_RemoveFacesWithoutSplines_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_RemoveFacesWithoutSplines;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_PhysicsEnabled_MetaData[];
#endif
		static void NewProp_PhysicsEnabled_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_PhysicsEnabled;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ForceDistribution_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ForceDistribution;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Stiffness_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Stiffness;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Damping_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Damping;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ConstantForce_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_ConstantForce;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MaxForce_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MaxForce;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MaxForceTorqueFactor_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MaxForceTorqueFactor;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReferenceHairBias_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReferenceHairBias;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_HairLengthForceUniformity_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_HairLengthForceUniformity;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MaxPhysicsOffsetLength_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_MaxPhysicsOffsetLength;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_NoiseStrength_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_NoiseStrength;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DisableMorphTargets_MetaData[];
#endif
		static void NewProp_DisableMorphTargets_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_DisableMorphTargets;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_StreamingDistanceMultiplier_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_StreamingDistanceMultiplier;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGFurComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UMeshComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UGFurComponent_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UGFurComponent_RegenerateFur, "RegenerateFur" }, // 2360648709
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Rendering" },
		{ "Comment", "/** UFurComponent */" },
		{ "DisplayName", "gFur" },
		{ "HideCategories", "Physics Object Activation Components|Activation Mobility Trigger" },
		{ "IncludePath", "FurComponent.h" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "UFurComponent" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData[] = {
		{ "Category", "gFur Skeletal Mesh" },
		{ "Comment", "/**\n\x09* Mesh used to generate shells. Usually this mesh will have the polys of eyes removed, interior of the mouth or claws.\n\x09* It's necessary for the mesh to be skinned exactly like the main mesh, otherwise different artifacts could show up.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Mesh used to generate shells. Usually this mesh will have the polys of eyes removed, interior of the mouth or claws.\nIt's necessary for the mesh to be skinned exactly like the main mesh, otherwise different artifacts could show up." },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh = { "SkeletalGrowMesh", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGrowMesh), Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData[] = {
		{ "Category", "gFur Static Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh = { "StaticGrowMesh", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGrowMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData[] = {
		{ "Category", "gFur Guides" },
		{ "Comment", "/**\n\x09* Splines from 3D apps that give basic shape and length to the fur. Does not have to be used.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Splines from 3D apps that give basic shape and length to the fur. Does not have to be used." },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines = { "FurSplines", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, FurSplines), Z_Construct_UClass_UFurSplines_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData)) };
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_Inner = { "SkeletalGuideMeshes", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData[] = {
		{ "Category", "gFur Skeletal Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes = { "SkeletalGuideMeshes", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGuideMeshes), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData)) };
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_Inner = { "StaticGuideMeshes", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData[] = {
		{ "Category", "gFur Static Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes = { "StaticGuideMeshes", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGuideMeshes), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ClampMax", "128" },
		{ "ClampMin", "1" },
		{ "Comment", "/**\n\x09* Sets the number of shells. Less = better performance\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Sets the number of shells. Less = better performance" },
		{ "UIMax", "128" },
		{ "UIMin", "1" },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount = { "LayerCount", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, LayerCount), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* If the fur becomes too small on the screen it will disappear for better performance. The lower the number the longer the fur remains on the screen even if small.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If the fur becomes too small on the screen it will disappear for better performance. The lower the number the longer the fur remains on the screen even if small." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize = { "MinScreenSize", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, MinScreenSize), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData)) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_Inner = { "LODs", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, Z_Construct_UScriptStruct_FFurLod, METADATA_PARAMS(nullptr, 0) }; // 2096731771
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs = { "LODs", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, LODs), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData)) }; // 2096731771
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->LODFromParent = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent = { "LODFromParent", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* With value 0.0 the shells are distributed linearly from root to tip. With values larger than 0.0, distribution becomes nonlinear,\n\x09* pushing the shells more to the tip where the shells tend to be more visible if the layer count is relatively low.\n\x09* With low shell count, using bias > 0 can help reduce artefacts at the fur tip.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value 0.0 the shells are distributed linearly from root to tip. With values larger than 0.0, distribution becomes nonlinear,\npushing the shells more to the tip where the shells tend to be more visible if the layer count is relatively low.\nWith low shell count, using bias > 0 can help reduce artefacts at the fur tip." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias = { "ShellBias", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, ShellBias), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* Sets the length of the fur if \"fur splines\" are not used. If \"fur splines\" are used, this value scales them (1.0 = no change, >1.0 splines are scaled up, <1.0 splines are scaled down)\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Sets the length of the fur if \"fur splines\" are not used. If \"fur splines\" are used, this value scales them (1.0 = no change, >1.0 splines are scaled up, <1.0 splines are scaled down)" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength = { "FurLength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, FurLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ClampMin", "0.001" },
		{ "Comment", "/**\n\x09* Overrides length of too short splines.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Overrides length of too short splines." },
		{ "UIMin", "0.001" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength = { "MinFurLength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, MinFurLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* Removes faces without splines.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Removes faces without splines." },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->RemoveFacesWithoutSplines = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines = { "RemoveFacesWithoutSplines", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* If fur should react to forces and movement.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If fur should react to forces and movement." },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->PhysicsEnabled = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled = { "PhysicsEnabled", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* With value = 1.0, the forces affecting fur are distributed linearly from root to tip.\n\x09* Values above 1.0 push the forces more to the tip, leaving the lower parts of fur strands less affected.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value = 1.0, the forces affecting fur are distributed linearly from root to tip.\nValues above 1.0 push the forces more to the tip, leaving the lower parts of fur strands less affected." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution = { "ForceDistribution", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, ForceDistribution), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* Higher values make the fur bend less under the different forces\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Higher values make the fur bend less under the different forces" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness = { "Stiffness", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, Stiffness), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* How fast the fur loses energy from different forces and comes to stop.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "How fast the fur loses energy from different forces and comes to stop." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping = { "Damping", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, Damping), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* Constant force affecting the fur. Usually this emulates gravity.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Constant force affecting the fur. Usually this emulates gravity." },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce = { "ConstantForce", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, ConstantForce), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* Maximum allowed forces to act on the fur. This allows you to clamp the motion of the fur if it's too much, but otherwise the fur looks good.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Maximum allowed forces to act on the fur. This allows you to clamp the motion of the fur if it's too much, but otherwise the fur looks good." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce = { "MaxForce", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForce), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* Clamps the forces coming from rotational movement of the bones. This value is multiplying the \"Max Force\" parameter.\n\x09* At value 1.0 torque forces are effectively clamped with \"Max Force\", values below 1.0 clamp the torque forces even more than \"Max Force\",\n\x09* values above 1.0 give more room to the torque forces that would be otherwise clamped if \"Max Force Torque Factor\" would be 1.0\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Clamps the forces coming from rotational movement of the bones. This value is multiplying the \"Max Force\" parameter.\nAt value 1.0 torque forces are effectively clamped with \"Max Force\", values below 1.0 clamp the torque forces even more than \"Max Force\",\nvalues above 1.0 give more room to the torque forces that would be otherwise clamped if \"Max Force Torque Factor\" would be 1.0" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor = { "MaxForceTorqueFactor", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForceTorqueFactor), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* Length of the reference hair for which physics is calculated for. 1 takes longer hair into account, 0 takes more of the short hair.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Length of the reference hair for which physics is calculated for. 1 takes longer hair into account, 0 takes more of the short hair." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias = { "ReferenceHairBias", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, ReferenceHairBias), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "Comment", "/**\n\x09* With value = 0.0, all the fur hair will bend closer to \"Reference Hair\". Values closer to 1.0 will bend longer and shorter hair the same way.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value = 0.0, all the fur hair will bend closer to \"Reference Hair\". Values closer to 1.0 will bend longer and shorter hair the same way." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity = { "HairLengthForceUniformity", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, HairLengthForceUniformity), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength = { "MaxPhysicsOffsetLength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, MaxPhysicsOffsetLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* Introduces noise to the shell vertices along the normal. This helps to break up the uniformity of the shell slices when viewed from the side.\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Introduces noise to the shell vertices along the normal. This helps to break up the uniformity of the shell slices when viewed from the side." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength = { "NoiseStrength", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, NoiseStrength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "Comment", "/**\n\x09* Turns off support for Morph Targets\n\x09*/" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Turns off support for Morph Targets" },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->DisableMorphTargets = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets = { "DisableMorphTargets", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_StreamingDistanceMultiplier_MetaData[] = {
		{ "Category", "SkeletalMesh" },
		{ "Comment", "/**\n\x09 * Allows adjusting the desired streaming distance of streaming textures that uses UV 0.\n\x09 * 1.0 is the default, whereas a higher value makes the textures stream in sooner from far away.\n\x09 * A lower value (0.0-1.0) makes the textures stream in later (you have to be closer).\n\x09 * Value can be < 0 (from legcay content, or code changes)\n\x09 */" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Allows adjusting the desired streaming distance of streaming textures that uses UV 0.\n1.0 is the default, whereas a higher value makes the textures stream in sooner from far away.\nA lower value (0.0-1.0) makes the textures stream in later (you have to be closer).\nValue can be < 0 (from legcay content, or code changes)" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StreamingDistanceMultiplier = { "StreamingDistanceMultiplier", nullptr, (EPropertyFlags)0x0010040000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UGFurComponent, StreamingDistanceMultiplier), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StreamingDistanceMultiplier_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StreamingDistanceMultiplier_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGFurComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODFromParent,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_DisableMorphTargets,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StreamingDistanceMultiplier,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGFurComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGFurComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UGFurComponent_Statics::ClassParams = {
		&UGFurComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UGFurComponent_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::PropPointers),
		0,
		0x00B010A4u,
		METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGFurComponent()
	{
		if (!Z_Registration_Info_UClass_UGFurComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UGFurComponent.OuterSingleton, Z_Construct_UClass_UGFurComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UGFurComponent.OuterSingleton;
	}
	template<> GFUR_API UClass* StaticClass<UGFurComponent>()
	{
		return UGFurComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGFurComponent);
	UGFurComponent::~UGFurComponent() {}
	struct Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics
	{
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ScriptStructInfo[] = {
		{ FFurLod::StaticStruct, Z_Construct_UScriptStruct_FFurLod_Statics::NewStructOps, TEXT("FurLod"), &Z_Registration_Info_UScriptStruct_FurLod, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FFurLod), 2096731771U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UGFurComponent, UGFurComponent::StaticClass, TEXT("UGFurComponent"), &Z_Registration_Info_UClass_UGFurComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UGFurComponent), 3511108984U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_4179484926(TEXT("/Script/GFur"),
		Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ClassInfo),
		Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_iwgne_OneDrive_Escritorio_gFur52RT_Fix_GFur_HostProject_Plugins_GFur_Source_GFur_Public_FurComponent_h_Statics::ScriptStructInfo),
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
