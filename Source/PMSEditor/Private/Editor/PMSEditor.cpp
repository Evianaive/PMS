#include "Editor/PMSEditor.h"
#include <Slate.h>
//#include "..\..\Public\Editor\PMSEditor.h"
//#include <Framework/Docking/TabManager.h>

#include "SGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphSchema.h"
#include "Editor/PMSEdGraphNode.h"
#include "Editor/SlateWidgets/SPMSGraphPanel.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphEditorImplPublic.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphEditorPublic.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphPanelPublic.h"

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
    InTabManager->UnregisterTabSpawner(PMSSpreadSheetTabId);
}

void FPMSEditor::InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UPMSEdGraph* InPMSGraphAsset)
{
    PMSGraph = InPMSGraphAsset;
    if (InPMSGraphAsset == nullptr)
    {
        // Todo 此处是否要指定Outer???
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

        

        InPMSGraphAsset = PMSGraph;
        //PMSGraphAsset->EdGraph = PMSGraph;
        //UPMSEdGraph* PMSEdGraph = Cast<UPMSEdGraph>(PMSGraphAsset->EdGraph);
        //PMSEdGraph->AddTestNode();
        //Schema->CreateDefaultNodesForGraph(*PMSGraphAsset->EdGraph);
    }
    else {
        PMSGraph = InPMSGraphAsset;
        PMSGraph->Schema = UPMSEdGraphSchema::StaticClass();
        //PMSGraph = PMSGraphAsset->EdGraph;
    }

    SGraphEditor::FGraphEditorEvents InGraphEvent;
    InGraphEvent.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FPMSEditor::OnSelectedPMSNodeChanged);
    // InGraphEvent.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateLambda([](){});
    if (InPMSGraphAsset != nullptr) {
        EdGraphEditor = SNew(SGraphEditor)
        .GraphToEdit(InPMSGraphAsset);
        // .GraphEvents(InGraphEvent);
        // InArgs = SGraphEditor::FArguments()
        //     .GraphToEdit(InPMSGraphAsset)
        //     .GraphEvents(InGraphEvent);
        //
        // MakeTDecl<SGraphEditor>( "SGraphEditor", __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs() ) . Expose( EdGraphEditor ) <<= TYPENAME_OUTSIDE_TEMPLATE InArgs;
        //
        //
        // //EdGraphEditor
        // SGraphEditorImpl* Implementation = (SGraphEditorImpl*)(((SGraphEditorPublic*)EdGraphEditor.Get())->Implementation.Get());
        // SGraphEditorImplPublic* ImplementationPublic = (SGraphEditorImplPublic*)Implementation;
        //
        // //((SGraphPanelFriend*)ImplementationPublic->GraphPanel.Get())->OnGetContextMenuFor;
        //
        // SPMSGraphPanel::FArguments NewGraphPanelArgs = SPMSGraphPanel::FArguments()
        // .GraphObj(InPMSGraphAsset)
        // .GraphObjToDiff(InArgs._GraphToDiff)
        // //.OnGetContextMenuFor( ImplementationPublic, &SGraphEditorImplPublic::GraphEd_OnGetContextMenuFor )
        // .OnSelectionChanged( InArgs._GraphEvents.OnSelectionChanged )
        // .OnNodeDoubleClicked( InArgs._GraphEvents.OnNodeDoubleClicked )
        // .IsEditable_Lambda( [this](){return InArgs._IsEditable.Get();} )
        // .DisplayAsReadOnly_Lambda( [this](){return InArgs._DisplayAsReadOnly.Get();} )
        // .OnDropActor( InArgs._GraphEvents.OnDropActor )
        // .OnDropStreamingLevel( InArgs._GraphEvents.OnDropStreamingLevel )
        // .OnVerifyTextCommit( InArgs._GraphEvents.OnVerifyTextCommit )
        // .OnTextCommitted( InArgs._GraphEvents.OnTextCommitted )
        // .OnSpawnNodeByShortcut( InArgs._GraphEvents.OnSpawnNodeByShortcut )
        // //.OnUpdateGraphPanel( this, &SGraphEditorImpl::GraphEd_OnPanelUpdated )
        // .OnDisallowedPinConnection( InArgs._GraphEvents.OnDisallowedPinConnection )
        // .ShowGraphStateOverlay(InArgs._ShowGraphStateOverlay)
        // .OnDoubleClicked(InArgs._GraphEvents.OnDoubleClicked);
        // NewGraphPanelArgs._OnGetContextMenuFor = ((SGraphPanelPublic*)ImplementationPublic->GraphPanel.Get())->OnGetContextMenuFor;
        //
        // MakeTDecl<SPMSGraphPanel>( "SPMSGraphPanel", __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs() ) . Expose( ImplementationPublic->GraphPanel ) <<= TYPENAME_OUTSIDE_TEMPLATE NewGraphPanelArgs;
        // ImplementationPublic->GraphPanel->RestoreViewSettings(FVector2D::ZeroVector, -1);
        // ImplementationPublic->GraphPanelSlot->AttachWidget(ImplementationPublic->GraphPanel.ToSharedRef());
    }
    //check(PMSGraphAsset->EdGraph != nullptr)
    /*Init Viewport*/
    PMSEditorViewport = SNew(SPMSEditorViewport)
    .PMSEditorPtr(SharedThis(this))
    .GraphToView(PMSGraph);

    /*Init SpreadSheet*/
    PMSEdtiorSpreadSheet = SNew(SScrollBox)
    .Orientation(EOrientation::Orient_Horizontal);
    
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
            //->SetSizeCoefficient(0.3f)
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
                ->SetSizeCoefficient(0.4f)
            )
            ->Split
            (
                FTabManager::NewStack()
                ->AddTab(PMSDetailsTabId, ETabState::OpenedTab)
                ->SetHideTabWell(true)
                ->SetSizeCoefficient(0.3f)
            )
        )
    );

    FAssetEditorToolkit::InitAssetEditor(InMode, InitToolkitHost, FName("GraphEditorIdentifier"), Layout, true, true, InPMSGraphAsset);
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