#include "Editor/PMSEditor.h"
#include <Slate.h>
//#include "..\..\Public\Editor\PMSEditor.h"
//#include <Framework/Docking/TabManager.h>

#include "Editor/PMSEdGraph.h"

#include "Editor/PMSEdGraphNode.h"

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
        PMSGraph = NewObject<UPMSEdGraph>();
        //PMSGraphAsset->EdGraph = NewObject<UEdGraph>(PMSGraphAsset,NAME_None, RF_Transactional);
        
        PMSGraph->bAllowDeletion = false;
        
        PMSGraph->Schema = UEdGraphSchema::StaticClass();
        //Give the schema a chance to fill out any required nodes (like the results node)
        const UEdGraphSchema* Schema = PMSGraph->GetSchema();
        
        //
        UPMSEdGraphNode * TestNode = NewObject<UPMSEdGraphNode>(PMSGraph);
        PMSGraph->Modify();
        TestNode->SetFlags(RF_Transactional);
        PMSGraph->AddNode(TestNode);
        PMSGraph->SelectNode = TestNode;

        

        PMSGraphAsset->EdGraph = Cast<UEdGraph>(PMSGraph);
        //UPMSEdGraph* PMSEdGraph = Cast<UPMSEdGraph>(PMSGraphAsset->EdGraph);
        //PMSEdGraph->AddTestNode();
        //Schema->CreateDefaultNodesForGraph(*PMSGraphAsset->EdGraph);
    }
    else {
        PMSGraph = Cast<UPMSEdGraph>(PMSGraphAsset->EdGraph);
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
#if test
    return SNew(SDockTab)[
        //ConstructIconsGallery()
        MakeStarshipGallery()
    ];
#else
    FDetailsViewArgs DetailsViewArgs;
    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bCustomNameAreaLocation = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    DetailsViewArgs.NotifyHook = this;

    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

    DetailsWidget = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    DetailsWidget->SetObject(PMSGraph->SelectNode);

    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            DetailsWidget.ToSharedRef()
        ];
#endif
}

#if test
//TSharedRef<SWidget> FPMSEditor::ConstructIconsGallery()
//{
//    // auto GenerateColorButton = [] (FTex      t InTitle, FName StyleColorName, bool Inverted = false) -> TSharedRef<SWidget> 
//
//    auto GenerateIconLibrary = [this](FText InTitle, FString InPath) -> TSharedRef<SWidget>
//    {
//        const FVector2D IconSize(20.f, 20.f);
//        TSharedPtr<SUniformWrapPanel> UniformWrapPanel = SNew(SUniformWrapPanel)
//            .HAlign(HAlign_Left)
//            .SlotPadding(FMargin(12.f, 12.f));
//
//        TArray<FString> FoundIcons;
//        FString SearchDirectory = FPaths::EngineDir() / InPath;// TEXT("Editor/Slate/Icons/GeneralTools");
//        // IFileManager::Get().FindFiles(FoundIcons, *SearchDirectory, TEXT(".png"));//, true, true, false);
//        IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDirectory, TEXT("*.png"), true, false);
//        for (const FString& Filename : FoundIcons)
//        {
//            // FString IconPath = SearchDirectory / Filename;
//            FString IconPath = Filename;
//
//            DynamicBrushes.Add(TUniquePtr<FSlateDynamicImageBrush>(new FSlateDynamicImageBrush(FName(*IconPath), IconSize)));
//
//            UniformWrapPanel->AddSlot()
//                [
//                    SNew(SImage)
//                    .Image(DynamicBrushes.Last().Get())
//                .ToolTipText(FText::FromString(IconPath))
//                ];
//        }
//
//        return SNew(SVerticalBox)
//            + SVerticalBox::Slot()
//            .AutoHeight()
//            [
//                SNew(STextBlock).Text(InTitle)
//            ]
//
//        + SVerticalBox::Slot()
//            [
//                UniformWrapPanel.ToSharedRef()
//            ];
//    };
//
//    auto GenerateIconLibrarySVG = [this](FText InTitle, FString InPath) -> TSharedRef<SWidget>
//    {
//        const FVector2D IconSize(20.f, 20.f);
//        TSharedPtr<SUniformWrapPanel> UniformWrapPanel = SNew(SUniformWrapPanel)
//            .HAlign(HAlign_Left)
//            .SlotPadding(FMargin(12.f, 12.f));
//
//        TArray<FString> FoundIcons;
//        FString SearchDirectory = FPaths::EngineDir() / InPath;// TEXT("Editor/Slate/Icons/GeneralTools");
//        // IFileManager::Get().FindFiles(FoundIcons, *SearchDirectory, TEXT(".png"));//, true, true, false);
//        IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDirectory, TEXT("*.svg"), true, false);
//        for (const FString& Filename : FoundIcons)
//        {
//            // FString IconPath = SearchDirectory / Filename;
//            FString IconPath = Filename;
//
//            DynamicBrushes.Add(TUniquePtr<FSlateVectorImageBrush>(new FSlateVectorImageBrush(IconPath, IconSize)));
//
//            UniformWrapPanel->AddSlot()
//                [
//                    SNew(SImage)
//                    .Image(DynamicBrushes.Last().Get())
//                .ToolTipText(FText::FromString(IconPath))
//                ];
//        }
//
//        return SNew(SVerticalBox)
//            + SVerticalBox::Slot()
//            .AutoHeight()
//            [
//                SNew(STextBlock).Text(InTitle)
//            ]
//
//        + SVerticalBox::Slot()
//            [
//                UniformWrapPanel.ToSharedRef()
//            ];
//    };
//
//
//
//    return SNew(SBorder)
//        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
//        [
//            SNew(SScrollBox)
//            + SScrollBox::Slot()
//        .Padding(48)
//        [
//            SNew(SVerticalBox)
//            + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "SlateCore", "Core"), "Content/Slate/Starship/Common")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "Editor Common", "Editor"), "Content/Editor/Slate/Starship/Common")]
//
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "SceneOutliner", "SceneOutliner"), "Content/Editor/Slate/Starship/SceneOutliner")]
//        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "LevelEditor", "LevelEditor"), "Content/Editor/Slate/Starship/LevelEditor/Menus")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "MainToolbar", "MainToolbar"), "Content/Editor/Slate/Starship/MainToolbar")]
//        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "FileMenu", "FileMenu"), "Content/Editor/Slate/Starship/Menus/File")]
//        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "EditMenu", "EditMenu"), "Content/Editor/Slate/Starship/Menus/Edit")]
//        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "HelpMenu", "HelpMenu"), "Content/Editor/Slate/Starship/Menus/Help")]
//
//        +SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "PaintIconTitle", "Paint"), "Content/Editor/Slate/Icons/Paint")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "LandscapeIconTitle", "Landscape"), "Content/Editor/Slate/Icons/Landscape")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "ModelingIconTitle", "Modeling"), "/Plugins/Experimental/ModelingToolsEditorMode/Content/Icons")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "FractureIconTitle", "Fracture"), "/Plugins/Experimental/ChaosEditor/Content")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "CurveEditorIconTitle", "CurveEditor"), "Content/Editor/Slate/GenericCurveEditor/Icons")]
//        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "GeneralIconTitle", "General"), "Content/Editor/Slate/Icons/GeneralTools")]
//        ]
//        ];
//}
#endif
#undef LOCTEXT_NAMESPACE