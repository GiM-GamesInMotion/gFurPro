// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GFur/Public/FurComponent.h"
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
	struct Z_Construct_UScriptStruct_FFurLod_Statics
	{
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PhysicsEnabled_MetaData[];
#endif
		static void NewProp_PhysicsEnabled_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_PhysicsEnabled;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Lod_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Lod;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ScreenSize_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ScreenSize;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UE4CodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FFurLod_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FFurLod>();
	}
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData[] = {
		{ "Category", "LOD" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If fur should react to forces and movement while using this LOD." },
	};
#endif
	void Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_SetBit(void* Obj)
	{
		((FFurLod*)Obj)->PhysicsEnabled = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled = { UE4CodeGen_Private::EPropertyClass::Bool, "PhysicsEnabled", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(FFurLod), &Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData[] = {
		{ "Category", "LOD" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Which LOD of the Grow Mesh should be used to generate fur for this LOD." },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod = { UE4CodeGen_Private::EPropertyClass::Int, "Lod", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, Lod), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData[] = {
		{ "Category", "LOD" },
		{ "ClampMax", "128" },
		{ "ClampMin", "1" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Number of shells for this particular LOD. Should be lower than \"Fur Layer Count\"" },
		{ "UIMax", "128" },
		{ "UIMin", "1" },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount = { UE4CodeGen_Private::EPropertyClass::Int, "LayerCount", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, LayerCount), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData[] = {
		{ "Category", "LOD" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Size of the fur on the screen at which the LOD will be used. The lower the number is, the longer it stays even if becomes smaller on the screen. Should be higher than \"Min Screen Size\"" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize = { UE4CodeGen_Private::EPropertyClass::Float, "ScreenSize", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(FFurLod, ScreenSize), METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_PhysicsEnabled,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_Lod,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_LayerCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FFurLod_Statics::NewProp_ScreenSize,
	};
	const UE4CodeGen_Private::FStructParams Z_Construct_UScriptStruct_FFurLod_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
		nullptr,
		&NewStructOps,
		"FurLod",
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		sizeof(FFurLod),
		alignof(FFurLod),
		Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::PropPointers),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams, ARRAY_COUNT(Z_Construct_UScriptStruct_FFurLod_Statics::Struct_MetaDataParams))
	};
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
			UE4CodeGen_Private::ConstructUScriptStruct(ReturnStruct, Z_Construct_UScriptStruct_FFurLod_Statics::ReturnStructParams);
		}
		return ReturnStruct;
	}
	uint32 Get_Z_Construct_UScriptStruct_FFurLod_CRC() { return 3717467212U; }
	void UGFurComponent::StaticRegisterNativesUGFurComponent()
	{
	}
	UClass* Z_Construct_UClass_UGFurComponent_NoRegister()
	{
		return UGFurComponent::StaticClass();
	}
	struct Z_Construct_UClass_UGFurComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_NoiseStrength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_NoiseStrength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxPhysicsOffsetLength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxPhysicsOffsetLength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_HairLengthForceUniformity_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_HairLengthForceUniformity;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ReferenceHairBias_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ReferenceHairBias;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxForceTorqueFactor_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxForceTorqueFactor;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MaxForce_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MaxForce;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ConstantForce_MetaData[];
#endif
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_ConstantForce;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Damping_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Damping;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Stiffness_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Stiffness;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ForceDistribution_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ForceDistribution;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_PhysicsEnabled_MetaData[];
#endif
		static void NewProp_PhysicsEnabled_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_PhysicsEnabled;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_RemoveFacesWithoutSplines_MetaData[];
#endif
		static void NewProp_RemoveFacesWithoutSplines_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_RemoveFacesWithoutSplines;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MinFurLength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MinFurLength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FurLength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_FurLength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ShellBias_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ShellBias;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LODs_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_LODs;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_LODs_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_MinScreenSize_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_MinScreenSize;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_LayerCount_MetaData[];
#endif
		static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_LayerCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticGuideMeshes_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_StaticGuideMeshes;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticGuideMeshes_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SkeletalGuideMeshes_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_SkeletalGuideMeshes;
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SkeletalGuideMeshes_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FurSplines_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_FurSplines;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_StaticGrowMesh_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_StaticGrowMesh;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_SkeletalGrowMesh_MetaData[];
#endif
		static const UE4CodeGen_Private::FObjectPropertyParams NewProp_SkeletalGrowMesh;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UGFurComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UMeshComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "ClassGroupNames", "Rendering" },
		{ "DisplayName", "gFur" },
		{ "HideCategories", "Collision Physics Object Activation Components|Activation Mobility Trigger" },
		{ "IncludePath", "FurComponent.h" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "UFurComponent" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Introduces noise to the shell vertices along the normal. This helps to break up the uniformity of the shell slices when viewed from the side." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength = { UE4CodeGen_Private::EPropertyClass::Float, "NoiseStrength", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, NoiseStrength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength = { UE4CodeGen_Private::EPropertyClass::Float, "MaxPhysicsOffsetLength", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MaxPhysicsOffsetLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value = 0.0, all the fur hair will bend closer to \"Reference Hair\". Values closer to 1.0 will bend longer and shorter hair the same way." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity = { UE4CodeGen_Private::EPropertyClass::Float, "HairLengthForceUniformity", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, HairLengthForceUniformity), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Length of the reference hair for which physics is calculated for. 1 takes longer hair into account, 0 takes more of the short hair." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias = { UE4CodeGen_Private::EPropertyClass::Float, "ReferenceHairBias", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ReferenceHairBias), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Clamps the forces coming from rotational movement of the bones. This value is multiplying the \"Max Force\" parameter.\nAt value 1.0 torque forces are effectively clamped with \"Max Force\", values below 1.0 clamp the torque forces even more than \"Max Force\",\nvalues above 1.0 give more room to the torque forces that would be otherwise clamped if \"Max Force Torque Factor\" would be 1.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor = { UE4CodeGen_Private::EPropertyClass::Float, "MaxForceTorqueFactor", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForceTorqueFactor), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Maximum allowed forces to act on the fur. This allows you to clamp the motion of the fur if it's too much, but otherwise the fur looks good." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce = { UE4CodeGen_Private::EPropertyClass::Float, "MaxForce", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MaxForce), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Constant force affecting the fur. Usually this emulates gravity." },
	};
#endif
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce = { UE4CodeGen_Private::EPropertyClass::Struct, "ConstantForce", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ConstantForce), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "How fast the fur loses energy from different forces and comes to stop." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping = { UE4CodeGen_Private::EPropertyClass::Float, "Damping", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, Damping), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Higher values make the fur bend less under the different forces" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness = { UE4CodeGen_Private::EPropertyClass::Float, "Stiffness", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, Stiffness), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value = 1.0, the forces affecting fur are distributed linearly from root to tip.\nValues above 1.0 push the forces more to the tip, leaving the lower parts of fur strands less affected." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution = { UE4CodeGen_Private::EPropertyClass::Float, "ForceDistribution", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ForceDistribution), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData[] = {
		{ "Category", "gFur Physics" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If fur should react to forces and movement." },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->PhysicsEnabled = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled = { UE4CodeGen_Private::EPropertyClass::Bool, "PhysicsEnabled", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Removes faces without splines." },
	};
#endif
	void Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_SetBit(void* Obj)
	{
		((UGFurComponent*)Obj)->RemoveFacesWithoutSplines = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines = { UE4CodeGen_Private::EPropertyClass::Bool, "RemoveFacesWithoutSplines", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UGFurComponent), &Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_SetBit, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ClampMin", "0.001" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Overrides length of too short splines." },
		{ "UIMin", "0.001" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength = { UE4CodeGen_Private::EPropertyClass::Float, "MinFurLength", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MinFurLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Sets the length of the fur if \"fur splines\" are not used. If \"fur splines\" are used, this value scales them (1.0 = no change, >1.0 splines are scaled up, <1.0 splines are scaled down)" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength = { UE4CodeGen_Private::EPropertyClass::Float, "FurLength", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, FurLength), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "With value 0.0 the shells are distributed linearly from root to tip. With values larger than 0.0, distribution becomes nonlinear,\npushing the shells more to the tip where the shells tend to be more visible if the layer count is relatively low.\nWith low shell count, using bias > 0 can help reduce artefacts at the fur tip." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias = { UE4CodeGen_Private::EPropertyClass::Float, "ShellBias", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, ShellBias), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs = { UE4CodeGen_Private::EPropertyClass::Array, "LODs", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, LODs), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "LODs", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FFurLod, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "If the fur becomes too small on the screen it will disappear for better performance. The lower the number the longer the fur remains on the screen even if small." },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize = { UE4CodeGen_Private::EPropertyClass::Float, "MinScreenSize", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, MinScreenSize), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData[] = {
		{ "Category", "gFur Shell settings" },
		{ "ClampMax", "128" },
		{ "ClampMin", "1" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Sets the number of shells. Less = better performance" },
		{ "UIMax", "128" },
		{ "UIMin", "1" },
	};
#endif
	const UE4CodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount = { UE4CodeGen_Private::EPropertyClass::Int, "LayerCount", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, LayerCount), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData[] = {
		{ "Category", "gFur Static Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes = { UE4CodeGen_Private::EPropertyClass::Array, "StaticGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGuideMeshes), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "StaticGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData[] = {
		{ "Category", "gFur Skeletal Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes = { UE4CodeGen_Private::EPropertyClass::Array, "SkeletalGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGuideMeshes), METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_MetaData)) };
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_Inner = { UE4CodeGen_Private::EPropertyClass::Object, "SkeletalGuideMeshes", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0000000000000000, 1, nullptr, 0, Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData[] = {
		{ "Category", "gFur Guides" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Splines from 3D apps that give basic shape and length to the fur. Does not have to be used." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines = { UE4CodeGen_Private::EPropertyClass::Object, "FurSplines", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, FurSplines), Z_Construct_UClass_UFurSplines_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData[] = {
		{ "Category", "gFur Static Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh = { UE4CodeGen_Private::EPropertyClass::Object, "StaticGrowMesh", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, StaticGrowMesh), Z_Construct_UClass_UStaticMesh_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData[] = {
		{ "Category", "gFur Skeletal Mesh" },
		{ "ModuleRelativePath", "Public/FurComponent.h" },
		{ "ToolTip", "Mesh used to generate shells. Usually this mesh will have the polys of eyes removed, interior of the mouth or claws.\nIt's necessary for the mesh to be skinned exactly like the main mesh, otherwise different artifacts could show up." },
	};
#endif
	const UE4CodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh = { UE4CodeGen_Private::EPropertyClass::Object, "SkeletalGrowMesh", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000005, 1, nullptr, STRUCT_OFFSET(UGFurComponent, SkeletalGrowMesh), Z_Construct_UClass_USkeletalMesh_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UGFurComponent_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_NoiseStrength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxPhysicsOffsetLength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_HairLengthForceUniformity,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ReferenceHairBias,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForceTorqueFactor,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MaxForce,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ConstantForce,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_Damping,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_Stiffness,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ForceDistribution,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_PhysicsEnabled,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_RemoveFacesWithoutSplines,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinFurLength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurLength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_ShellBias,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LODs_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_MinScreenSize,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_LayerCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGuideMeshes_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGuideMeshes_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_FurSplines,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_StaticGrowMesh,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UGFurComponent_Statics::NewProp_SkeletalGrowMesh,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UGFurComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UGFurComponent>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UGFurComponent_Statics::ClassParams = {
		&UGFurComponent::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x00B010A0u,
		nullptr, 0,
		Z_Construct_UClass_UGFurComponent_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::PropPointers),
		nullptr,
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UGFurComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UGFurComponent()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UGFurComponent_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UGFurComponent, 3537322388);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UGFurComponent(Z_Construct_UClass_UGFurComponent, &UGFurComponent::StaticClass, TEXT("/Script/GFur"), TEXT("UGFurComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UGFurComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
