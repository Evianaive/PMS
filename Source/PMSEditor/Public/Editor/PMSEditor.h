// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

//FWorkflowCentricApplication
#include <WorkflowOrientedApp/WorkflowCentricApplication.h>

//FEditorUndoClient
#include <EditorUndoClient.h>

//FNotifyHook
#include <Misc/NotifyHook.h>

//
//#include <Framework/Docking/TabManager.h>

//FPMSEditor contains a UPMSGraph
#include "PMSGraph.h"

class PMSEDITOR_API FPMSEditor : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	void InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UPMS_Graph* InPMSGraphAsset);
	//void InitPMSEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UPMS* InObject);

public:
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	//virtual void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager) override;

	// ͨ�� FWorkflowCentricApplication �̳�
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;

private:
	static const FName PMSDetailsTabId;
	static const FName PMSGraphTabId;

	UPMS_Graph* PMSGraphAsset;
	TSharedPtr<SGraphEditor> EdGraphEditor;
	TSharedPtr<IDetailsView> DetailsWidget;

	TSharedRef<SDockTab> SpawnTab_UpdateGraph(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
};