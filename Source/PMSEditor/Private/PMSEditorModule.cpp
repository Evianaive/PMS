// Copyright Epic Games, Inc. All Rights Reserved.

#include "PMSEditorModule.h"

#include <IAssetTools.h>
#include <Modules/ModuleManager.h>
#include <AssetToolsModule.h>

#include "AssetTypeActions/AssetTypeActions_PMS.h"

#define LOCTEXT_NAMESPACE "FPMSEditorModule"

void FPMSEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeActions_PMS> AssetTypeAction = MakeShareable(new FAssetTypeActions_PMS());
	AssetToolsModule.RegisterAssetTypeActions(AssetTypeAction.ToSharedRef());
}

void FPMSEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPMSEditorModule, PMSEditor)