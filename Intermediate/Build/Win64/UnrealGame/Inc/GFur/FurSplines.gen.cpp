// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GFur/Public/FurSplines.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurSplines() {}
// Cross Module References
	GFUR_API UClass* Z_Construct_UClass_UFurSplines_NoRegister();
	GFUR_API UClass* Z_Construct_UClass_UFurSplines();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_GFur();
	COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
// End Cross Module References
	void UFurSplines::StaticRegisterNativesUFurSplines()
	{
	}
	UClass* Z_Construct_UClass_UFurSplines_NoRegister()
	{
		return UFurSplines::StaticClass();
	}
	struct Z_Construct_UClass_UFurSplines_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_Vertices_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Vertices_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_Vertices;
		static const UECodeGen_Private::FIntPropertyParams NewProp_Index_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Index_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_Index;
		static const UECodeGen_Private::FIntPropertyParams NewProp_Count_Inner;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Count_MetaData[];
#endif
		static const UECodeGen_Private::FArrayPropertyParams NewProp_Count;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ControlPointCount_MetaData[];
#endif
		static const UECodeGen_Private::FIntPropertyParams NewProp_ControlPointCount;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ImportFilename_MetaData[];
#endif
		static const UECodeGen_Private::FStrPropertyParams NewProp_ImportFilename;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Version_MetaData[];
#endif
		static const UECodeGen_Private::FIntPropertyParams NewProp_Version;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ImportTransformation_MetaData[];
#endif
		static const UECodeGen_Private::FIntPropertyParams NewProp_ImportTransformation;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Threshold_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Threshold;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFurSplines_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "FurSplines.h" },
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_Inner = { "Vertices", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices = { "Vertices", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Vertices), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData)) };
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_Inner = { "Index", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData[] = {
		{ "Comment", "// Old - don't use\n" },
		{ "ModuleRelativePath", "Public/FurSplines.h" },
		{ "ToolTip", "Old - don't use" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Index = { "Index", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Index), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData)) };
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_Inner = { "Count", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Count = { "Count", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Count), EArrayPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount = { "ControlPointCount", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, ControlPointCount), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename = { "ImportFilename", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, ImportFilename), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Version = { "Version", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Version), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportTransformation_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportTransformation = { "ImportTransformation", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, ImportTransformation), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportTransformation_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportTransformation_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Threshold_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Threshold = { "Threshold", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Threshold), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Threshold_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Threshold_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFurSplines_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Index,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_Inner,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Count,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Version,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportTransformation,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Threshold,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurSplines_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurSplines>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UFurSplines_Statics::ClassParams = {
		&UFurSplines::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UFurSplines_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UFurSplines()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UECodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UFurSplines_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurSplines, 59682008);
	template<> GFUR_API UClass* StaticClass<UFurSplines>()
	{
		return UFurSplines::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurSplines(Z_Construct_UClass_UFurSplines, &UFurSplines::StaticClass, TEXT("/Script/GFur"), TEXT("UFurSplines"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurSplines);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
