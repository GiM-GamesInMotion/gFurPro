// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GFurEditor/Private/FurSplineAbcImporterFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeFurSplineAbcImporterFactory() {}
// Cross Module References
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurSplineAbcImporterFactory_NoRegister();
	GFUREDITOR_API UClass* Z_Construct_UClass_UFurSplineAbcImporterFactory();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_GFurEditor();
// End Cross Module References
	void UFurSplineAbcImporterFactory::StaticRegisterNativesUFurSplineAbcImporterFactory()
	{
	}
	UClass* Z_Construct_UClass_UFurSplineAbcImporterFactory_NoRegister()
	{
		return UFurSplineAbcImporterFactory::StaticClass();
	}
	struct Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_GFurEditor,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "FurSplineAbcImporterFactory.h" },
		{ "ModuleRelativePath", "Private/FurSplineAbcImporterFactory.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UFurSplineAbcImporterFactory>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::ClassParams = {
		&UFurSplineAbcImporterFactory::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x000000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UFurSplineAbcImporterFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_UFurSplineAbcImporterFactory_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UFurSplineAbcImporterFactory, 1787767853);
	template<> GFUREDITOR_API UClass* StaticClass<UFurSplineAbcImporterFactory>()
	{
		return UFurSplineAbcImporterFactory::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_UFurSplineAbcImporterFactory(Z_Construct_UClass_UFurSplineAbcImporterFactory, &UFurSplineAbcImporterFactory::StaticClass, TEXT("/Script/GFurEditor"), TEXT("UFurSplineAbcImporterFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UFurSplineAbcImporterFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
