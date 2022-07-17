// Copyright Epic Games, Inc. All Rights Reserved.

#include "PMSEditorModule.h"

#include <IAssetTools.h>
#include <Modules/ModuleManager.h>
#include <AssetToolsModule.h>

#include "ISettingsModule.h"
#include "AssetTypeActions/AssetTypeActions_PMS.h"
#include "Editor/PMSEdGraphSchema.h"
#include "Editor/PMSEditorSettings.h"
#include "Editor/SlateWidgets/PMSGraphEditorWidgets/SGraphEditor_PMS.h"
#include "Editor/Style/PMSEditorStyle.h"
#include "Editor/Utilities/FNodeShape.h"
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"
#include "Editor/SlateWidgets/PMSGraphEditorWidgets/SGraphEditorImpl_PMS.h"

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
	FNodeShapeCollection::Initialize();
	UPMSEdGraphSchema::Init(true);
	
	//Register InputPreProcessor hacker
	if(FSlateApplication::IsInitialized())
	{
		PMSInputPreProcessor = MakeShareable(new FPMSEdGraphPanelInputPreProcessor());
		bool bPreProcessor = FSlateApplication::Get().RegisterInputPreProcessor(PMSInputPreProcessor);
		if(bPreProcessor)
		{
			//Todo 设置一些参数，如hover距离，按键方面的一些东西，可以注册config setting
		}
	}
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Editor", "Plugins", "PMS",
			NSLOCTEXT("PMS","SettingsName", "Procedual Modeling System"),
			NSLOCTEXT("PMS","SettingsDescription", "Procedual Modeling System Setting"),
			GetMutableDefault<UPMSEditorSettings>());
	}

	TArray< TWeakPtr<SGraphEditor_PMS> >& Instances = SGraphEditor_PMS::AllInstances_PMS;
	for (auto InstanceIt = SGraphEditor_PMS::AllInstances_PMS.CreateIterator(); InstanceIt; ++InstanceIt)
	{
		TWeakPtr<SGraphEditor_PMS>& Instance = *InstanceIt;
		if (Instance.IsValid())
		{
			Instance.Pin()->OnModuleReloaded();
		}		
	}
}

void FPMSEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//Todo 此处需要Unregister AssetTypeAction?
	
	//Unregister Style
	FPMSEditorStyle::Shutdown();
	FNodeShapeCollection::Shutdown();

	//Unregister InputPreProcessor hacker
	if(PMSInputPreProcessor.IsValid() && FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(PMSInputPreProcessor);
		PMSInputPreProcessor.Reset();
		//Todo 如果前面注册了config setting 这里需要unregister
	}

	// Notify all the instances of GraphEditor that their code is about to be unloaded.
	for (auto InstanceIt = SGraphEditor_PMS::AllInstances_PMS.CreateIterator(); InstanceIt; ++InstanceIt)
	{
		TWeakPtr<SGraphEditor_PMS>& Instance = *InstanceIt;
		if (Instance.IsValid())
		{
			Instance.Pin()->OnModuleUnloading();
		}		
	}
}

TSharedRef<SGraphEditor_PMS> FPMSEditorModule::PRIVATE_MakeGraphEditor_PMS(
	const TSharedPtr<FUICommandList>& InAdditionalCommands, const TAttribute<bool>& InIsEditable,
	const TAttribute<bool>& InDisplayAsReadOnly, const TAttribute<bool>& InIsEmpty,
	TAttribute<FGraphAppearanceInfo> Appearance, TSharedPtr<SWidget> InTitleBar, UEdGraph* InGraphToEdit,
	SGraphEditor::FGraphEditorEvents InGraphEvents, bool InAutoExpandActionMenu, UEdGraph* InGraphToDiff,
	FSimpleDelegate InOnNavigateHistoryBack, FSimpleDelegate InOnNavigateHistoryForward,
	TAttribute<bool> ShowGraphStateOverlay)
{
	return
		SNew(SGraphEditorImpl_PMS)
		.AdditionalCommands(InAdditionalCommands)
		.IsEditable(InIsEditable)
		.DisplayAsReadOnly(InDisplayAsReadOnly)
		.Appearance(Appearance)
		.TitleBar(InTitleBar)
		.GraphToEdit(InGraphToEdit)
		.GraphEvents(InGraphEvents)
		.AutoExpandActionMenu(InAutoExpandActionMenu)
		.GraphToDiff(InGraphToDiff)
		.OnNavigateHistoryBack(InOnNavigateHistoryBack)
		.OnNavigateHistoryForward(InOnNavigateHistoryForward)
		.ShowGraphStateOverlay(ShowGraphStateOverlay);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPMSEditorModule, PMSEditor)