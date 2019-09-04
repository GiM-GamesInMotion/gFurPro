// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
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
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Version_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Version;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ImportFilename_MetaData[];
#endif
		static const UE4CodeGen_Private::FStrPropertyParams NewProp_ImportFilename;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ControlPointCount_MetaData[];
#endif
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_ControlPointCount;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Count_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Count;
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Count_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Index_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Index;
		static const UE4CodeGen_Private::FIntPropertyParams NewProp_Index_Inner;
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Vertices_MetaData[];
#endif
		static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Vertices;
		static const UE4CodeGen_Private::FStructPropertyParams NewProp_Vertices_Inner;
		static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFurSplines_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_GFur,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "FurSplines.h" },
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Version = { "Version", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Version), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Version_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UE4CodeGen_Private::FStrPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename = { "ImportFilename", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, ImportFilename), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount = { "ControlPointCount", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, ControlPointCount), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount_MetaData)) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Count = { "Count", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Count), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_MetaData)) };
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_Inner = { "Count", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData[] = {
		{ "Comment", "// Old - don't use\n" },
		{ "ModuleRelativePath", "Public/FurSplines.h" },
		{ "ToolTip", "Old - don't use" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Index = { "Index", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Index), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_MetaData)) };
	const UE4CodeGen_Private::FIntPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_Inner = { "Index", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData[] = {
		{ "ModuleRelativePath", "Public/FurSplines.h" },
	};
#endif
	const UE4CodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices = { "Vertices", nullptr, (EPropertyFlags)0x0010000000000000, UE4CodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UFurSplines, Vertices), METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_MetaData)) };
	const UE4CodeGen_Private::FStructPropertyParams Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_Inner = { "Vertices", nullptr, (EPropertyFlags)0x0000000000000000, UE4CodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, 0, Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(nullptr, 0) };
	const UE4CodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UFurSplines_Statics::PropPointers[] = {
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Version,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_ImportFilename,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_ControlPointCount,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Count,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Count_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Index,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Index_Inner,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices,
		(const UE4CodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UFurSplines_Statics::NewProp_Vertices_Inner,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurSplines_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurSplines>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UFurSplines_Statics::ClassParams = {
		&UFurSplines::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UFurSplines_Statics::PropPointers,
		nullptr,
		ARRAY_COUNT(DependentSingletons),
		0,
		ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UFurSplines_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UFurSplines()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UFurSplines_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurSplines, 2389362597);
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
