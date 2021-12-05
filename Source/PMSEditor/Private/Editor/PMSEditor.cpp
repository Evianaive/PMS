#include "Editor/PMSEditor.h"
#include <Slate.h>
//#include "..\..\Public\Editor\PMSEditor.h"
//#include <Framework/Docking/TabManager.h>

#include "SGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphSchema.h"
#include "Editor/PMSEdGraphNode.h"

#define LOCTEXT_NAMESPACE "ProceduralModelingSystemEditor"

const FName FPMSEditor::PMSDetailsTabId(TEXT("PMSDetailsTabId"));
const FName FPMSEditor::PMSGraphTabId(TEXT("PMSGraphTabId"));
const FName FPMSEditor::PMSViewportTabId(TEXT("PMSViewportTabId"));
const FName FPMSEditor::PMSSpreadSheetTabId(TEXT("PMSGeoSpreadSheetTabId"));

void FPMSEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_PMSEditor", "Procedural Modeling System Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

    FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(PMSGraphTabId, FOnSpawnTab::CreateSP(this, &FPMSEditor::SpawnTab_UpdateGraph))
        .SetDisplayName(LOCTEXT("Graph", "Graph"))
        .SetGroup(WorkspaceMenuCategoryRef);

    InTabManager->RegisterTabSpawner(PMSDetailsTabId, FOnSpawnTab::CreateSP(this, &FPMSEditor::SpawnTab_Details))
        .SetDisplayName(LOCTEXT("Details", "Details"))
        .SetGroup(WorkspaceMenuCategoryRef)
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
    InTabManager->RegisterTabSpawner(PMSViewportTabId, FOnSpawnTab::CreateSP(this, &FPMSEditor::SpawnTab_Viewport))
        .SetDisplayName(LOCTEXT("Viewport","Viewport"))
        .SetGroup(WorkspaceMenuCategoryRef);
    InTabManager->RegisterTabSpawner(PMSSpreadSheetTabId, FOnSpawnTab::CreateSP(this, &FPMSEditor::SpawnTab_SpreadSheet))
        .SetDisplayName(LOCTEXT("SpreadSheet","SpreadSheet"))
        .SetGroup(WorkspaceMenuCategoryRef);
}

void FPMSEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
    FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);
    //FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

    InTabManager->UnregisterTabSpawner(PMSDetailsTabId);
    InTabManager->UnregisterTabSpawner(PMSGraphTabId);
    InTabManager->UnregisterTabSpawner(PMSViewportTabId);
}

void FPMSEditor::InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UPMS_Graph* InPMSGraphAsset)
{
    PMSGraphAsset = InPMSGraphAsset;
    if (PMSGraphAsset->EdGraph == nullptr)
    {
        PMSGraph = NewObject<UPMSEdGraph>();
        //PMSGraphAsset->EdGraph = NewObject<UEdGraph>(PMSGraphAsset,NAME_None, RF_Transactional);
        
        PMSGraph->bAllowDeletion = false;
        
        PMSGraph->Schema = UPMSEdGraphSchema::StaticClass();
        //Give the schema a chance to fill out any required nodes (like the results node)
        const UEdGraphSchema* Schema = PMSGraph->GetSchema();
        
        //
        // UPMSEdGraphNode * TestNode = NewObject<UPMSEdGraphNode>(PMSGraph);
        // PMSGraph->Modify();
        // TestNode->SetFlags(RF_Transactional);
        // PMSGraph->AddNode(TestNode);
        // PMSGraph->SelectNode = TestNode;

        

        PMSGraphAsset->EdGraph = Cast<UEdGraph>(PMSGraph);
        //PMSGraphAsset->EdGraph = PMSGraph;
        //UPMSEdGraph* PMSEdGraph = Cast<UPMSEdGraph>(PMSGraphAsset->EdGraph);
        //PMSEdGraph->AddTestNode();
        //Schema->CreateDefaultNodesForGraph(*PMSGraphAsset->EdGraph);
    }
    else {
        PMSGraph = Cast<UPMSEdGraph>(PMSGraphAsset->EdGraph);
        //PMSGraph = PMSGraphAsset->EdGraph;
    }

    SGraphEditor::FGraphEditorEvents InGraphEvent;
    InGraphEvent.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FPMSEditor::OnSelectedPMSNodeChanged);
    if (PMSGraphAsset->EdGraph != nullptr) {
        EdGraphEditor = SNew(SGraphEditor)
            .GraphToEdit(PMSGraphAsset->EdGraph)
        .GraphEvents(InGraphEvent);
    }
    //check(PMSGraphAsset->EdGraph != nullptr)
    /*Init Viewport*/
    PMSEditorViewport = SNew(SPMSEditorViewport)
    .PMSEditorPtr(SharedThis(this))
    .GraphToView(PMSGraph);

    /*Init SpreadSheet*/
    PMSEdtiorSpreadSheet = SNew(SScrollBox);
    
    TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("PMS_Test")
    ->AddArea
    (
        FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
        //->Split
        //(
        //    FTabManager::NewStack()
        //    ->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
        //    ->SetHideTabWell(true)
        //    ->SetSizeCoefficient(0.2f)
        //)
        ->Split
        (
            FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
                ->Split
                (
                    FTabManager::NewStack()
                    ->AddTab(PMSViewportTabId,ETabState::OpenedTab)
                    ->SetHideTabWell(true)
                    ->SetSizeCoefficient(0.5f)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->AddTab(PMSSpreadSheetTabId,ETabState::OpenedTab)
                    ->SetHideTabWell(true)
                    ->SetSizeCoefficient(0.5f)
                )
            )
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab(PMSGraphTabId, ETabState::OpenedTab)
                ->SetHideTabWell(true)
                ->SetSizeCoefficient(0.45f)
            )
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab(PMSDetailsTabId, ETabState::OpenedTab)
                ->SetHideTabWell(true)
                ->SetSizeCoefficient(0.25f)
            )
        )
    );

    FAssetEditorToolkit::InitAssetEditor(InMode, InitToolkitHost, FName("GraphEditorIdentifier"), Layout, true, true, PMSGraphAsset);
}

FLinearColor FPMSEditor::GetWorldCentricTabColorScale() const
{
    return FLinearColor();
}

FName FPMSEditor::GetToolkitFName() const
{
    return FName("PMSEditor");
}

FText FPMSEditor::GetBaseToolkitName() const
{
    return NSLOCTEXT("PMS_Toolkit", "PMSEditor", "Procedural Modeling System Editor");
}

FString FPMSEditor::GetWorldCentricTabPrefix() const
{
    return FString();
}

TSharedRef<SDockTab> FPMSEditor::SpawnTab_UpdateGraph(const FSpawnTabArgs& Args) {
    check(Args.GetTabId() == PMSGraphTabId);
    
    check(EdGraphEditor);
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            EdGraphEditor.ToSharedRef()
        ];
}
TSharedRef<SDockTab> FPMSEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PMSViewportTabId);

    check(PMSEditorViewport);
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            PMSEditorViewport.ToSharedRef()         
        ];
}
TSharedRef<SDockTab> FPMSEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PMSDetailsTabId);

    FDetailsViewArgs DetailsViewArgs;
    //remeber to check this starts
    DetailsViewArgs.bUpdatesFromSelection = false;
    //remeber to check this ends
    DetailsViewArgs.bCustomNameAreaLocation = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.NotifyHook = this;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    DetailsWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsWidget->OnFinishedChangingProperties().AddSP(this,&FPMSEditor::OnFinishedChangingPMSProperties);
    DetailsWidget->SetObject(nullptr);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            DetailsWidget.ToSharedRef()
        ];
}
TSharedRef<SDockTab> FPMSEditor::SpawnTab_SpreadSheet(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PMSSpreadSheetTabId);
    check(PMSEdtiorSpreadSheet);
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            PMSEdtiorSpreadSheet.ToSharedRef()
        ];
}

void FPMSEditor::OnSelectedPMSNodeChanged(const TSet<class UObject*>& SelectionNode)
{
    if(SelectionNode.Num() > 0)
    {
        TArray<class UObject*> SelectionNodeArray = SelectionNode.Array();
        UObject* NodeObjectToShow = SelectionNodeArray[0];
        int32 MinPosX = Cast<UEdGraphNode>(NodeObjectToShow)->NodePosX; 
        for(auto& CurIterObject:SelectionNodeArray)
        {
            UEdGraphNode* CurIterNode = Cast<UEdGraphNode>(CurIterObject);
            if(MinPosX > CurIterNode->NodePosX)
            {
                MinPosX = CurIterNode->NodePosX;
                NodeObjectToShow = CurIterObject;
            }
        }
        DetailsWidget->SetObject(NodeObjectToShow);
    }
}

void FPMSEditor::OnFinishedChangingPMSProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
    //TSharedPtr<UPMSEdGraphNode> Node = DetailsWidget->GetSelectedObjects()[0];
    
    for (int32 Index = 0; Index < PropertyChangedEvent.GetNumObjectsBeingEdited(); ++Index)
    {
        const UObject* EditedObject = PropertyChangedEvent.GetObjectBeingEdited(Index);
        Cast<UPMSEdGraphNode>(EditedObject)->SlateNode->UpdateGraphNode();
    }
    
}
#undef LOCTEXT_NAMESPACE