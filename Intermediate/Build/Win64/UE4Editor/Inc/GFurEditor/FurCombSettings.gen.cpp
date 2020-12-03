// Copyright Epic Games, Inc. All Rights Reserved.
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
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Radius_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Radius;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Strength_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_Strength;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_FalloffAmount_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_FalloffAmount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplyHeight_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplyHeight;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ApplySpread_MetaData[];
#endif
		static const UE4CodeGen_Private::FFloatPropertyParams NewProp_ApplySpread;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bEnableFlow_MetaData[];
#endif
		static void NewProp_bEnableFlow_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bEnableFlow;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorX_MetaData[];
#endif
		static void NewProp_bMirrorX_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorX;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorY_MetaData[];
#endif
		static void NewProp_bMirrorY_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorY;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bMirrorZ_MetaData[];
#endif
		static void NewProp_bMirrorZ_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bMirrorZ;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_bShowSplines_MetaData[];
#endif
		static void NewProp_bShowSplines_SetBit(void* Obj);
		static const UE4CodeGen_Private::FBoolPropertyParams NewProp_bShowSplines;
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
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1000000.0" },
		{ "ClampMin", "0.01" },
		{ "Comment", "/** Radius of the Brush used for Painting */" },
		{ "DisplayName", "Radius" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Radius of the Brush used for Painting" },
		{ "UIMax", "50.0" },
		{ "UIMin", "0.01" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius = { "Radius", nullptr, (EPropertyFlags)0x0020080000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurCombSettings, Radius), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1000.0" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** Strength of the brush (0.0 - 1.0) */" },
		{ "DisplayName", "Strength" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Strength of the brush (0.0 - 1.0)" },
		{ "UIMax", "5.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength = { "Strength", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurCombSettings, Strength), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** Amount of falloff to apply (0.0 - 1.0) */" },
		{ "DisplayName", "Falloff" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Amount of falloff to apply (0.0 - 1.0)" },
		{ "UIMax", "1.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount = { "FalloffAmount", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurCombSettings, FalloffAmount), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** Height of fur spline at which the brush effect is applied. 0=root, 1=tip */" },
		{ "DisplayName", "Effect Height" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Height of fur spline at which the brush effect is applied. 0=root, 1=tip" },
		{ "UIMax", "1.0" },
		{ "UIMin", "0.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight = { "ApplyHeight", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurCombSettings, ApplyHeight), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData[] = {
		{ "Category", "Comb" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "-1.0" },
		{ "Comment", "/** Affects the distance at which the effect is applied or faded out in both directions from the \"Effect Height\" point. -1 = extremely short, mostly at exact location defined by \"Effect Height\", 0 smooth distribution which fades out in both directions from \"Effect Height\", 1 affects the whole spline same way, achieving sort of linear effect.*/" },
		{ "DisplayName", "Effect Distribution" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Affects the distance at which the effect is applied or faded out in both directions from the \"Effect Height\" point. -1 = extremely short, mostly at exact location defined by \"Effect Height\", 0 smooth distribution which fades out in both directions from \"Effect Height\", 1 affects the whole spline same way, achieving sort of linear effect." },
		{ "UIMax", "1.0" },
		{ "UIMin", "-1.0" },
	};
#endif
	const UE4CodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread = { "ApplySpread", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurCombSettings, ApplySpread), METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData[] = {
		{ "Category", "Comb" },
		{ "Comment", "/** If the brush should affect the fur even if it's not moved.*/" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "If the brush should affect the fur even if it's not moved." },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bEnableFlow = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow = { "bEnableFlow", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData[] = {
		{ "Category", "Comb" },
		{ "Comment", "/** Mirrors the grooming along the X axis of the mesh.*/" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Mirrors the grooming along the X axis of the mesh." },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorX = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX = { "bMirrorX", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData[] = {
		{ "Category", "Comb" },
		{ "Comment", "/** Mirrors the grooming along the Y axis of the mesh.*/" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Mirrors the grooming along the Y axis of the mesh." },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorY = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY = { "bMirrorY", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData[] = {
		{ "Category", "Comb" },
		{ "Comment", "/** Mirrors the grooming along the Z axis of the mesh.*/" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Mirrors the grooming along the Z axis of the mesh." },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bMirrorZ = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ = { "bMirrorZ", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData[] = {
		{ "Category", "Comb" },
		{ "Comment", "/** Toggles visualization of the spline guides.*/" },
		{ "ModuleRelativePath", "Private/FurCombSettings.h" },
		{ "ToolTip", "Toggles visualization of the spline guides." },
	};
#endif
	void Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_SetBit(void* Obj)
	{
		((UFurCombSettings*)Obj)->bShowSplines = 1;
	}
	const UE4CodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines = { "bShowSplines", nullptr, (EPropertyFlags)0x0010000000000001, UE4CodeGen_Private::EPropertyGenFlags::Bool | UE4CodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, sizeof(bool), sizeof(UFurCombSettings), &Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_SetBit, METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines_MetaData)) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFurCombSettings_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Radius,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_Strength,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_FalloffAmount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplyHeight,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_ApplySpread,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bEnableFlow,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorX,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorY,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bMirrorZ,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurCombSettings_Statics::NewProp_bShowSplines,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurCombSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurCombSettings>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UFurCombSettings_Statics::ClassParams = {
		&UFurCombSettings::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UFurCombSettings_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UFurCombSettings_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UFurCombSettings_Statics::Class_MetaDataParams))
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
	IMPLEMENT_CLASS(UFurCombSettings, 2898775594);
	template<> GFUREDITOR_API UClass* StaticClass<UFurCombSettings>()
	{
		return UFurCombSettings::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurCombSettings(Z_Construct_UClass_UFurCombSettings, &UFurCombSettings::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurCombSettings"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurCombSettings);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
