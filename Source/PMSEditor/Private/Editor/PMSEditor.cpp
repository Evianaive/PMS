#include "Editor/PMSEditor.h"
#include <Slate.h>
//#include "..\..\Public\Editor\PMSEditor.h"
//#include <Framework/Docking/TabManager.h>

#define LOCTEXT_NAMESPACE "ProceduralModelingSystemEditor"

const FName FPMSEditor::PMSDetailsTabId(TEXT("PMSDetailsTabId"));
const FName FPMSEditor::PMSGraphTabId(TEXT("PMSGraphTabId"));

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
}

void FPMSEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
    FWorkflowCentricApplication::UnregisterTabSpawners(InTabManager);
    //FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

    InTabManager->UnregisterTabSpawner(PMSDetailsTabId);
    InTabManager->UnregisterTabSpawner(PMSGraphTabId);
}

void FPMSEditor::InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UPMS_Graph* InPMSGraphAsset)
{
    PMSGraphAsset = InPMSGraphAsset;
    if (PMSGraphAsset->EdGraph == nullptr)
    {
        PMSGraphAsset->EdGraph = NewObject<UEdGraph>(PMSGraphAsset,NAME_None, RF_Transactional);
        PMSGraphAsset->EdGraph->bAllowDeletion = false;
        PMSGraphAsset->EdGraph->Schema = UEdGraphSchema::StaticClass();
        //Give the schema a chance to fill out any required nodes (like the results node)
        const UEdGraphSchema* Schema = PMSGraphAsset->EdGraph->GetSchema();
        //Schema->CreateDefaultNodesForGraph(*PMSGraphAsset->EdGraph);
    }
    //check(PMSGraphAsset->EdGraph != nullptr)

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
                    FTabManager::NewStack()
                    ->AddTab(PMSGraphTabId, ETabState::OpenedTab)
                    ->SetHideTabWell(true)
                    ->SetSizeCoefficient(0.85f)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->AddTab(PMSDetailsTabId, ETabState::OpenedTab)
                    ->SetHideTabWell(true)
                    ->SetSizeCoefficient(0.15f)
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
    
    if (PMSGraphAsset->EdGraph != nullptr) {
        EdGraphEditor = SNew(SGraphEditor)
            .GraphToEdit(PMSGraphAsset->EdGraph);
    }
    else {

    }
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            EdGraphEditor.ToSharedRef()
        ];
}
TSharedRef<SDockTab> FPMSEditor::SpawnTab_Details(const FSpawnTabArgs& Args)
{
    check(Args.GetTabId() == PMSDetailsTabId);
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bCustomNameAreaLocation = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.NotifyHook = this;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    DetailsWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsWidget->SetObject(PMSGraphAsset);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            DetailsWidget.ToSharedRef()
        ];
}
#undef LOCTEXT_NAMESPACE