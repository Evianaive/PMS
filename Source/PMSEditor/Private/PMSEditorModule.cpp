// Copyright Epic Games, Inc. All Rights Reserved.

#include "PMSEditorModule.h"

#include <IAssetTools.h>
#include <Modules/ModuleManager.h>
#include <AssetToolsModule.h>

#include "ISettingsModule.h"
#include "AssetTypeActions/AssetTypeActions_PMS.h"
#include "Editor/PMSEditorSettings.h"
#include "Editor/Style/PMSEditorStyle.h"
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"

#define LOCTEXT_NAMESPACE "FPMSEditorModule"

void FPMSEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//Register Asset Type
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeActions_PMS> AssetTypeAction = MakeShareable(new FAssetTypeActions_PMS());
	AssetToolsModule.RegisterAssetTypeActions(AssetTypeAction.ToSharedRef());

	//Register Style
	FPMSEditorStyle::Initialize();
	FPMSEditorStyle::ReloadTextures();

	//Register InputPreProcessor hacker
	if(FSlateApplication::IsInitialized())
	{
		PMSInputPreProcessor = MakeShareable(new FPMSEdGraphPanelInputPreProcessor());
		bool bPreProcessor = FSlateApplication::Get().RegisterInputPreProcessor(PMSInputPreProcessor);
		if(bPreProcessor)
		{
			//Todo ����һЩ��������hover���룬���������һЩ����������ע��config setting
		}
	}
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "PMS",
			NSLOCTEXT("PMS","SettingsName", "Procedual Modeling System"),
			NSLOCTEXT("PMS","SettingsDescription", "Procedual Modeling System Setting"),
			GetMutableDefault<UPMSEditorSettings>());
	}
}

void FPMSEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//Todo �˴���ҪUnregister AssetTypeAction?
	
	//Unregister Style
	FPMSEditorStyle::Shutdown();

	//Unregister InputPreProcessor hacker
	if(PMSInputPreProcessor.IsValid() && FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(PMSInputPreProcessor);
		PMSInputPreProcessor.Reset();
		//Todo ���ǰ��ע����config setting ������Ҫunregister
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPMSEditorModule, PMSEditor)