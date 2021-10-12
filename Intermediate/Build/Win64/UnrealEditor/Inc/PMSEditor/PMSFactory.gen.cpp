// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "PMSEditor/Public/AssetFactory/PMSFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePMSFactory() {}
// Cross Module References
	PMSEDITOR_API UClass* Z_Construct_UClass_UPMSFactory_NoRegister();
	PMSEDITOR_API UClass* Z_Construct_UClass_UPMSFactory();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_PMSEditor();
// End Cross Module References
	void UPMSFactory::StaticRegisterNativesUPMSFactory()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPMSFactory);
	UClass* Z_Construct_UClass_UPMSFactory_NoRegister()
	{
		return UPMSFactory::StaticClass();
	}
	struct Z_Construct_UClass_UPMSFactory_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UPMSFactory_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UFactory,
		(UObject* (*)())Z_Construct_UPackage__Script_PMSEditor,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPMSFactory_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Object" },
		{ "IncludePath", "AssetFactory/PMSFactory.h" },
		{ "ModuleRelativePath", "Public/AssetFactory/PMSFactory.h" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UPMSFactory_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPMSFactory>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UPMSFactory_Statics::ClassParams = {
		&UPMSFactory::StaticClass,
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
		0x000800A0u,
		METADATA_PARAMS(Z_Construct_UClass_UPMSFactory_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UPMSFactory_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UPMSFactory()
	{
		if (!Z_Registration_Info_UClass_UPMSFactory.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPMSFactory.OuterSingleton, Z_Construct_UClass_UPMSFactory_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UPMSFactory.OuterSingleton;
	}
	template<> PMSEDITOR_API UClass* StaticClass<UPMSFactory>()
	{
		return UPMSFactory::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPMSFactory);
	struct Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMSEditor_Public_AssetFactory_PMSFactory_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMSEditor_Public_AssetFactory_PMSFactory_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UPMSFactory, UPMSFactory::StaticClass, TEXT("UPMSFactory"), &Z_Registration_Info_UClass_UPMSFactory, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPMSFactory), 445068961U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMSEditor_Public_AssetFactory_PMSFactory_h_3907228470(TEXT("/Script/PMSEditor"),
		Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMSEditor_Public_AssetFactory_PMSFactory_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMSEditor_Public_AssetFactory_PMSFactory_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
