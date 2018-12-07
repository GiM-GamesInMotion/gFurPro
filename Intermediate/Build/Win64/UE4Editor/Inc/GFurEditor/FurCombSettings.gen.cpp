// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GFurEditor/Private/FurCombSettings.h"
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
	struct Z_Construct_UClass_UFurCombSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShowSplines_MetaData[];
#endif
		static void NewProp_bShowSplines_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShowSplines;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorZ_MetaData[];
#endif
		static void NewProp_bMirrorZ_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorZ;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorY_MetaData[];
#endif
		static void NewProp_bMirrorY_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorY;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorX_MetaData[];
#endif
		static void NewProp_bMirrorX_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorX;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bEnableFlow_MetaData[];
#endif
		static void NewProp_bEnableFlow_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEnableFlow;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplySpread_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplySpread;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplyHeight_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplyHeight;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FalloffAmount_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_FalloffAmount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Strength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Strength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Radius_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Radius;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFurCombSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GFurEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "FurCombSettings.h" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData[] = {
		{ "Category", "Comb" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bShowSplines = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines = { UE4CodeGen_Private::EPropertyClass::Bool, "bShowSplines", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData[] = {
		{ "Category", "Comb" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorZ = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorZ", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData[] = {
		{ "Category", "Comb" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorY = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorY", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData[] = {
		{ "Category", "Comb" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorX = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX = { UE4CodeGen_Private::EPropertyClass::Bool, "bMirrorX", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData[] = {
		{ "Category", "Comb" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bEnableFlow = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow = { UE4CodeGen_Private::EPropertyClass::Bool, "bEnableFlow", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, sizeof(bool), UE4CodeGen_Private::ENativeBool::Native, sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "-1.0" },
		{ "DisplayName", "Apply Spread" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "UIMax", "1.0" },
		{ "UIMin", "-1.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread = { UE4CodeGen_Private::EPropertyClass::Float, "ApplySpread", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, ApplySpread), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "DisplayName", "Apply Height" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "UIMax", "1.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight = { UE4CodeGen_Private::EPropertyClass::Float, "ApplyHeight", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, ApplyHeight), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData[] = {
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
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount = { UE4CodeGen_Private::EPropertyClass::Float, "FalloffAmount", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, FalloffAmount), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1000.0" },
		{ "ClampMin", "0.0" },
		{ "DisplayName", "Strength" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Strength of the brush (0.0 - 1.0)" },
		{ "UIMax", "1.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength = { UE4CodeGen_Private::EPropertyClass::Float, "Strength", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, Strength), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData[] = {
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
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius = { UE4CodeGen_Private::EPropertyClass::Float, "Radius", RF_Public|RF_Transient|RF_MarkAsNative, (EPropertyFlags)0x0020080000000001, 1, nullptr, STRUCT_OFFSET(UFurCombSettings, Radius), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFurCombSettings_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurCombSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurCombSettings>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UFurCombSettings_Statics::ClassParams = {
		&UFurCombSettings::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x001000A0u,
		nullptr, 0,
		Z_Construct_UClass_UFurCombSettings_Statics::PropPointers, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::PropPointers),
		nullptr,
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UFurCombSettings()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UFurCombSettings_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurCombSettings, 605000040);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurCombSettings(Z_Construct_UClass_UFurCombSettings, &UFurCombSettings::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurCombSettings"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurCombSettings);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
