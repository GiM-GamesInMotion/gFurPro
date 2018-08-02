// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/FurSplines.h"
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
	UClass* Z_Construct_UClass_UFurSplines()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UObject,
				(UObject* (*)())Z_Construct_UPackage__Script_GFur,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "FurSplines.h" },
				{ "ModuleRelativePath", "Public/FurSplines.h" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_ImportFilename_MetaData[] = {
				{ "ModuleRelativePath", "Public/FurSplines.h" },
			};
#endif
			static const UE4CodeGen_Private::FStrPropertyParams NewProp_ImportFilename = { UE4CodeGen_Private::EPropertyClass::Str, "ImportFilename", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(UFurSplines, ImportFilename), METADATA_PARAMS(NewProp_ImportFilename_MetaData, ARRAY_COUNT(NewProp_ImportFilename_MetaData)) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Count_MetaData[] = {
				{ "ModuleRelativePath", "Public/FurSplines.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Count = { UE4CodeGen_Private::EPropertyClass::Array, "Count", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(UFurSplines, Count), METADATA_PARAMS(NewProp_Count_MetaData, ARRAY_COUNT(NewProp_Count_MetaData)) };
			static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Count_Inner = { UE4CodeGen_Private::EPropertyClass::Int, "Count", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Index_MetaData[] = {
				{ "ModuleRelativePath", "Public/FurSplines.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Index = { UE4CodeGen_Private::EPropertyClass::Array, "Index", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(UFurSplines, Index), METADATA_PARAMS(NewProp_Index_MetaData, ARRAY_COUNT(NewProp_Index_MetaData)) };
			static const UE4CodeGen_Private::FUnsizedIntPropertyParams NewProp_Index_Inner = { UE4CodeGen_Private::EPropertyClass::Int, "Index", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Vertices_MetaData[] = {
				{ "ModuleRelativePath", "Public/FurSplines.h" },
			};
#endif
			static const UE4CodeGen_Private::FArrayPropertyParams NewProp_Vertices = { UE4CodeGen_Private::EPropertyClass::Array, "Vertices", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000000, 1, nullptr, STRUCT_OFFSET(UFurSplines, Vertices), METADATA_PARAMS(NewProp_Vertices_MetaData, ARRAY_COUNT(NewProp_Vertices_MetaData)) };
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_Vertices_Inner = { UE4CodeGen_Private::EPropertyClass::Struct, "Vertices", RF_Public|RF_Transient|RF_MarkAsNative, 0x0000000000000000, 1, nullptr, 0, Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(nullptr, 0) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_ImportFilename,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Count,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Count_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Index,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Index_Inner,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Vertices,
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Vertices_Inner,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UFurSplines>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UFurSplines::StaticClass,
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
	IMPLEMENT_CLASS(UFurSplines, 1534991963);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurSplines(Z_Construct_UClass_UFurSplines, &UFurSplines::StaticClass, TEXT("/Script/GFur"), TEXT("UFurSplines"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurSplines);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
