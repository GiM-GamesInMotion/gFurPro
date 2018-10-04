// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GFurEditor/Private/FurSplineImporterFactory.h"
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
	struct Z_Construct_UClass_UFurSplineImporterFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFurSplineImporterFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_GFurEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplineImporterFactory_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "FurSplineImporterFactory.h" },
		{ "ModuleRelativePath", "Private/FurSplineImporterFactory.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurSplineImporterFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurSplineImporterFactory>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UFurSplineImporterFactory_Statics::ClassParams = {
		&UFurSplineImporterFactory::StaticClass,
		DependentSingletons, ARRAY_COUNT(DependentSingletons),
		0x000000A0u,
		nullptr, 0,
		nullptr, 0,
		nullptr,
		&StaticCppClassTypeInfo,
		nullptr, 0,
		METADATA_PARAMS(Z_Construct_UClass_UFurSplineImporterFactory_Statics::Class_MetaDataParams, ARRAY_COUNT(Z_Construct_UClass_UFurSplineImporterFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UFurSplineImporterFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UFurSplineImporterFactory_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurSplineImporterFactory, 1558158309);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurSplineImporterFactory(Z_Construct_UClass_UFurSplineImporterFactory, &UFurSplineImporterFactory::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurSplineImporterFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurSplineImporterFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
