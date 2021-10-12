// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "PMS/Public/PMSGraph.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodePMSGraph() {}
// Cross Module References
	PMS_API UClass* Z_Construct_UClass_UPMS_Graph_NoRegister();
	PMS_API UClass* Z_Construct_UClass_UPMS_Graph();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_PMS();
	ENGINE_API UClass* Z_Construct_UClass_UEdGraph_NoRegister();
// End Cross Module References
	void UPMS_Graph::StaticRegisterNativesUPMS_Graph()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UPMS_Graph);
	UClass* Z_Construct_UClass_UPMS_Graph_NoRegister()
	{
		return UPMS_Graph::StaticClass();
	}
	struct Z_Construct_UClass_UPMS_Graph_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_EdGraph_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_EdGraph;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UPMS_Graph_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_PMS,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPMS_Graph_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "IncludePath", "PMSGraph.h" },
		{ "IsBlueprintBase", "true" },
		{ "ModuleRelativePath", "Public/PMSGraph.h" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UPMS_Graph_Statics::NewProp_EdGraph_MetaData[] = {
		{ "ModuleRelativePath", "Public/PMSGraph.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UPMS_Graph_Statics::NewProp_EdGraph = { "EdGraph", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, STRUCT_OFFSET(UPMS_Graph, EdGraph), Z_Construct_UClass_UEdGraph_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UPMS_Graph_Statics::NewProp_EdGraph_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UPMS_Graph_Statics::NewProp_EdGraph_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UPMS_Graph_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UPMS_Graph_Statics::NewProp_EdGraph,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UPMS_Graph_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UPMS_Graph>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UPMS_Graph_Statics::ClassParams = {
		&UPMS_Graph::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UPMS_Graph_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UPMS_Graph_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(Z_Construct_UClass_UPMS_Graph_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UPMS_Graph_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UPMS_Graph()
	{
		if (!Z_Registration_Info_UClass_UPMS_Graph.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UPMS_Graph.OuterSingleton, Z_Construct_UClass_UPMS_Graph_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UPMS_Graph.OuterSingleton;
	}
	template<> PMS_API UClass* StaticClass<UPMS_Graph>()
	{
		return UPMS_Graph::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UPMS_Graph);
	struct Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMS_Public_PMSGraph_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMS_Public_PMSGraph_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UPMS_Graph, UPMS_Graph::StaticClass, TEXT("UPMS_Graph"), &Z_Registration_Info_UClass_UPMS_Graph, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UPMS_Graph), 3799010004U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMS_Public_PMSGraph_h_843619084(TEXT("/Script/PMS"),
		Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMS_Public_PMSGraph_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_ProceduralModeling_Plugins_PMS_Source_PMS_Public_PMSGraph_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
