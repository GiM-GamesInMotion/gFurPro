// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/FurSplineImporterFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurSplineImporterFactory() {}
// Cross Module References
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurSplineImporterFactory_NoRegister();
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurSplineImporterFactory();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_GFurEditor();
// End Cross Module References
	void UFurSplineImporterFactory::StaticRegisterNativesUFurSplineImporterFactory()
	{
	}
	UClass* Z_Construct_UClass_UFurSplineImporterFactory_NoRegister()
	{
		return UFurSplineImporterFactory::StaticClass();
	}
	UClass* Z_Construct_UClass_UFurSplineImporterFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UFactory,
				(UObject* (*)())Z_Construct_UPackage__Script_GFurEditor,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "FurSplineImporterFactory.h" },
				{ "ModuleRelativePath", "Private/FurSplineImporterFactory.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UFurSplineImporterFactory>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UFurSplineImporterFactory::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00000080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurSplineImporterFactory, 2565954681);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurSplineImporterFactory(Z_Construct_UClass_UFurSplineImporterFactory, &UFurSplineImporterFactory::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurSplineImporterFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurSplineImporterFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
