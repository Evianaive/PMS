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
#include "Editor/PMSEdGraph.h"
#include "Viewport/SPMSViewportClient.h"
#include "Widgets/Layout/SScrollBox.h"

class FPMSEditor : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public:
	void InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UPMS_Graph* InPMSGraphAsset);
	//void InitPMSEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UPMS* InObject);

public:
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager) override;
	//virtual void RegisterToolbarTab(const TSharedRef<class FTabManager>& TabManager) override;

	// 通过 FWorkflowCentricApplication 继承
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;

private:
	UPMS_Graph* PMSGraphAsset;
	UPMSEdGraph* PMSGraph;
	
	static const FName PMSDetailsTabId;
	static const FName PMSGraphTabId;
	static const FName PMSViewportTabId;
	static const FName PMSSpreadSheetTabId;
	
	TSharedPtr<SGraphEditor> EdGraphEditor;
	SGraphEditor::FArguments InArgs;
	TSharedPtr<IDetailsView> DetailsWidget;
	TSharedPtr<SPMSEditorViewport> PMSEditorViewport;
	/*需要重新实现*/
	TSharedPtr<SScrollBox> PMSEdtiorSpreadSheet;
	// TSharedPtr<SHorizontalBox> Hirechy;

	TSharedRef<SDockTab> SpawnTab_UpdateGraph(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_SpreadSheet(const FSpawnTabArgs& Args);

	void OnSelectedPMSNodeChanged(const TSet<class UObject*>& SelectionNode);
	void OnFinishedChangingPMSProperties(const FPropertyChangedEvent& InEvent);
	void UpdateEditorByGraph(UPMSEdGraph* InGraph);
	void OnTryOpenSubGraph(UEdGraphNode* InNode);
};
