// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/FurCombSettings.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurCombSettings() {}
// Cross Module References
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurCombSettings_NoRegister();
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurCombSettings();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_GFurEditor();
// End Cross Module References
	void UFurCombSettings::StaticRegisterNativesUFurCombSettings()
	{
	}
	UClass* Z_Construct_UClass_UFurCombSettings_NoRegister()
	{
		return UFurCombSettings::StaticClass();
	}
	UClass* Z_Construct_UClass_UFurCombSettings()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UObject,
				(UObject* (*)())Z_Construct_UPackage__Script_GFurEditor,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "FurCombSettings.h" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShowSplines_MetaData[] = {
				{ "Category", "Comb" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Toggles visualization of the spline guides." },
			};
#endif
			auto NewProp_bShowSplines_SetBit = [](void* Obj){ ((UFurCombSettings*)Obj)->bShowSplines = 1; };
			static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShowSplines = { UE4CodeGen_Private::EPropertyClass::Bool, "bShowSplines", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &UE4CodeGen_Private::TBoolSetBitWrapper<decltype(NewProp_bShowSplines_SetBit)>::SetBit, METADATA_PARAMS(NewProp_bShowSplines_MetaData, ARRAY_COUNT(NewProp_bShowSplines_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorZ_MetaData[] = {
				{ "Category", "Comb" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Mirrors the grooming along the Z axis of the mesh." },
			};
#endif
			auto NewProp_bMirrorZ_SetBit = [](void* Obj){ ((UFurCombSettings*)Obj)->bMirrorZ = 1; };
			static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorZ = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorZ", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &UE4CodeGen_Private::TBoolSetBitWrapper<decltype(NewProp_bMirrorZ_SetBit)>::SetBit, METADATA_PARAMS(NewProp_bMirrorZ_MetaData, ARRAY_COUNT(NewProp_bMirrorZ_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorY_MetaData[] = {
				{ "Category", "Comb" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Mirrors the grooming along the Y axis of the mesh." },
			};
#endif
			auto NewProp_bMirrorY_SetBit = [](void* Obj){ ((UFurCombSettings*)Obj)->bMirrorY = 1; };
			static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorY = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorY", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &UE4CodeGen_Private::TBoolSetBitWrapper<decltype(NewProp_bMirrorY_SetBit)>::SetBit, METADATA_PARAMS(NewProp_bMirrorY_MetaData, ARRAY_COUNT(NewProp_bMirrorY_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorX_MetaData[] = {
				{ "Category", "Comb" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Mirrors the grooming along the X axis of the mesh." },
			};
#endif
			auto NewProp_bMirrorX_SetBit = [](void* Obj){ ((UFurCombSettings*)Obj)->bMirrorX = 1; };
			static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorX = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorX", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &UE4CodeGen_Private::TBoolSetBitWrapper<decltype(NewProp_bMirrorX_SetBit)>::SetBit, METADATA_PARAMS(NewProp_bMirrorX_MetaData, ARRAY_COUNT(NewProp_bMirrorX_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bEnableFlow_MetaData[] = {
				{ "Category", "Comb" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "If the brush should affect the fur even if it's not moved." },
			};
#endif
			auto NewProp_bEnableFlow_SetBit = [](void* Obj){ ((UFurCombSettings*)Obj)->bEnableFlow = 1; };
			static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEnableFlow = { UE4CodeGen_Private::EPropertyClass::Bool, "bEnableFlow", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &UE4CodeGen_Private::TBoolSetBitWrapper<decltype(NewProp_bEnableFlow_SetBit)>::SetBit, METADATA_PARAMS(NewProp_bEnableFlow_MetaData, ARRAY_COUNT(NewProp_bEnableFlow_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplySpread_MetaData[] = {
				{ "Category", "Comb" },
				{ "ClampMax", "1.0" },
				{ "ClampMin", "-1.0" },
				{ "DisplayName", "Effect Distribution" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Affects the distance at which the effect is applied or faded out in both directions from the \"Effect Height\" point. -1 = extremely short, mostly at exact location defined by \"Effect Height\", 0 smooth distribution which fades out in both directions from \"Effect Height\", 1 affects the whole spline same way, achieving sort of linear effect." },
				{ "UIMax", "1.0" },
				{ "UIMin", "-1.0" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplySpread = { UE4CodeGen_Private::EPropertyClass::Float, "ApplySpread", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, ApplySpread), METADATA_PARAMS(NewProp_ApplySpread_MetaData, ARRAY_COUNT(NewProp_ApplySpread_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplyHeight_MetaData[] = {
				{ "Category", "Comb" },
				{ "ClampMax", "1.0" },
				{ "ClampMin", "0.0" },
				{ "DisplayName", "Effect Height" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Height of fur spline at which the brush effect is applied. 0=root, 1=tip" },
				{ "UIMax", "1.0" },
				{ "UIMin", "0.0" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplyHeight = { UE4CodeGen_Private::EPropertyClass::Float, "ApplyHeight", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, ApplyHeight), METADATA_PARAMS(NewProp_ApplyHeight_MetaData, ARRAY_COUNT(NewProp_ApplyHeight_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FalloffAmount_MetaData[] = {
				{ "Category", "Comb" },
				{ "ClampMax", "1.0" },
				{ "ClampMin", "0.0" },
				{ "DisplayName", "Falloff" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Amount of falloff to apply (0.0 - 1.0)" },
				{ "UIMax", "1.0" },
				{ "UIMin", "0.0" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_FalloffAmount = { UE4CodeGen_Private::EPropertyClass::Float, "FalloffAmount", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, FalloffAmount), METADATA_PARAMS(NewProp_FalloffAmount_MetaData, ARRAY_COUNT(NewProp_FalloffAmount_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Strength_MetaData[] = {
				{ "Category", "Comb" },
				{ "ClampMax", "1000.0" },
				{ "ClampMin", "0.0" },
				{ "DisplayName", "Strength" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Strength of the brush (0.0 - 1.0)" },
				{ "UIMax", "5.0" },
				{ "UIMin", "0.0" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Strength = { UE4CodeGen_Private::EPropertyClass::Float, "Strength", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, Strength), METADATA_PARAMS(NewProp_Strength_MetaData, ARRAY_COUNT(NewProp_Strength_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Radius_MetaData[] = {
				{ "Category", "Comb" },
				{ "ClampMax", "1000000.0" },
				{ "ClampMin", "0.01" },
				{ "DisplayName", "Radius" },
				{ "ModuleRelativePath", "Private/FurCombSettings.h" },
				{ "ToolTip", "Radius of the Brush used for Painting" },
				{ "UIMax", "50.0" },
				{ "UIMin", "0.01" },
			};
#endif
			static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Radius = { UE4CodeGen_Private::EPropertyClass::Float, "Radius", RF_Public|RF_Transient|RF_MarkAsNative, 0x0020080000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, Radius), METADATA_PARAMS(NewProp_Radius_MetaData, ARRAY_COUNT(NewProp_Radius_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_bShowSplines,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_bMirrorZ,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_bMirrorY,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_bMirrorX,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_bEnableFlow,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ApplySpread,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ApplyHeight,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_FalloffAmount,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Strength,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Radius,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UFurCombSettings>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UFurCombSettings::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00100080u,
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
	IMPLEMENT_CLASS(UFurCombSettings, 602624843);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurCombSettings(Z_Construct_UClass_UFurCombSettings, &UFurCombSettings::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurCombSettings"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurCombSettings);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
