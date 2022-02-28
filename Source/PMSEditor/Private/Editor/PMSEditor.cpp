#include "Editor/PMSEditor.h"
#include <Slate.h>
//#include "..\..\Public\Editor\PMSEditor.h"
//#include <Framework/Docking/TabManager.h>

#include "SGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphSchema.h"
#include "Editor/PMSEdGraphNode.h"
#include "Editor/PMSEdSubGraphNode.h"
#include "Editor/SlateWidgets/SPMSGraphPanel.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphEditorImplPublic.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphEditorPublic.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphPanelPublic.h"
#include "Editor/Style/PMSEditorStyle.h"

#include "GeomTools.h"
#include "Editor/SlateWidgets/S2DMeshWidget.h"
#include "Json.h"
#include "ToolMenu.h"
#include "Editor/PMSEditorSettings.h"
#include "Slate/SlateVectorArtData.h"
#include "Slate/SMeshWidget.h"
// #include "AppFramework/Private/Widgets/Testing/STestSuite.cpp"

#define LOCTEXT_NAMESPACE "ProceduralModelingSystemEditor"

const FName FPMSEditor::PMSDetailsTabId(TEXT("PMSDetailsTabId"));
const FName FPMSEditor::PMSGraphTabId(TEXT("PMSGraphTabId"));
const FName FPMSEditor::PMSViewportTabId(TEXT("PMSViewportTabId"));
const FName FPMSEditor::PMSSpreadSheetTabId(TEXT("PMSGeoSpreadSheetTabId"));

void PushMenu()
{
	// FMenuBuilder MenuBuilder(true, nullptr);
	// MenuBuilder.BeginSection("PathFolderMoveCopy", FText::FromString(TEXT("Test Begin")));
	// {
	// 	MenuBuilder.AddMenuEntry(
	// 		  FText::FromString(TEXT("Test Entry0")),
	// 		  FText::FromString(TEXT("Test Entry1")),
	// 		  FSlateIcon(),
	// 		  FUIAction(FExecuteAction::CreateLambda([]() { }))
	// 	);
	// }
	// MenuBuilder.EndSection();
 //              
	// TSharedPtr< SWindow > Parent = FSlateApplication::Get().GetActiveTopLevelWindow();
	// if (Parent.IsValid())
	// {
	// 	FSlateApplication::Get().PushMenu(
	// 		  Parent.ToSharedRef(),
	// 		  FWidgetPath(),
	// 		  MenuBuilder.MakeWidget(),
	// 		  FSlateApplication::Get().GetCursorPos(),
	// 		   FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
	// 	);
	// }
	FSoftObjectPath Asset = FSoftObjectPath("/Game/NewSlateVectorArtData.NewSlateVectorArtData");
	UObject* Object = Asset.ResolveObject();
	USlateVectorArtData* VectorArtData = Cast<USlateVectorArtData>(Object);
	
	auto SMesh = SNew(SMeshWidget).MeshData(VectorArtData);

	
	
	FString NodeShape = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/NodeShapes/light.json");
	// TArray<TArray<FClipSMTriangle>> Shapes = StaticMeshFromJson(NodeShape);

	// auto textBlock = SNew(STextBlock).Text(FText::FromString("Test"));
	// auto TextLeafWidget = SNew(STestLeafWidget); 
	// auto S2dMeshWidget = SNew(S2DMeshWidget)
	// .MeshData(Shapes[0])
	// .RenderTransform(FVector2D(12.0f,12.0f))
	;

	
	
	auto testWindow = SNew(SWindow)
	.Title(LOCTEXT("Asset Window", "Asset Window"))
	.ClientSize(FVector2D(30, 20))
	.SupportsMaximize(false)
	.SupportsMinimize(false)
	.SizingRule(ESizingRule::Autosized)
	.AutoCenter(EAutoCenter::None)
	.IsInitiallyMaximized(false)
	.bDragAnywhere(true)
	[
		// SMesh
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		[
			SNew(S2DMeshWidget)
			.MeshData(FNodeShapeCollection::Get()->GetNodeShapeByName(*(GetDefault<UPMSEditorSettings>()->NameToDebug.Find("NodeShapeName"))))
		]
		// + SHorizontalBox::Slot()
		// [
		// 	SNew(S2DMeshWidget)
		// 	.MeshData(Shapes[1])
		// ]
		// + SHorizontalBox::Slot()
		// [
		// 	SNew(S2DMeshWidget)
		// 	.MeshData(Shapes[2])
		// ]
		// + SHorizontalBox::Slot()
		// [
		// 	SNew(S2DMeshWidget)
		// 	.MeshData(Shapes[3])
		// ]
		// SNew(SElementTesting)
	];
 
	TSharedPtr<SWindow> TopWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (TopWindow.IsValid())
	{
		//Add as Native
		FSlateApplication::Get().AddWindowAsNativeChild(testWindow, TopWindow.ToSharedRef(), true);
	}
	else
	{
 
		//Default in case no top window
		FSlateApplication::Get().AddWindow(testWindow);
	}
}

void FPMSEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
    WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_PMSEditor", "Procedural Modeling System Editor"));
    auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

    FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);

    InTabManager->RegisterTabSpawner(PMSGraphTabId, FOnSpawnTab::CreateSP(this, &FPMSEditor::SpawnTab_GraphEditor))
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

void FPMSEditor::InitPMSAssetEditor(const EToolkitMode::Type InMode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UPMS_Graph* InPMSGraphAsset)
{
    PMSGraphAsset = InPMSGraphAsset;
    if (PMSGraphAsset->EdGraph == nullptr)
    {
        PMSGraph = NewObject<UPMSEdGraph>(PMSGraphAsset);
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
    InGraphEvent.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this,&FPMSEditor::OnTryOpenSubGraph);
	// InGraphEvent.OnDoubleClicked = SGraphEditor::FOnDoubleClicked::CreateStatic(&PushMenu);
	InGraphEvent.OnCreateActionMenu = SGraphEditor::FOnCreateActionMenu::CreateSP(this,&FPMSEditor::OnGetContextMenu);
    // InGraphEvent.OnVerifyTextCommit = FOnNodeVerifyTextCommit::CreateLambda([](){});

    GraphEditor = SNew(SVerticalBox);

    /* SGraphEditor::FArguments().Appearance(FGraphAppearanceInfo) */
    
    InArgs = SGraphEditor::FArguments()
        .GraphEvents(InGraphEvent);
    UpdateEditorByGraph(PMSGraph);
    
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

TSharedRef<SDockTab> FPMSEditor::SpawnTab_GraphEditor(const FSpawnTabArgs& Args) {
    check(Args.GetTabId() == PMSGraphTabId);
    
    check(GraphEditor);
    return SNew(SDockTab)
        .TabRole(ETabRole::PanelTab)
        [
            GraphEditor.ToSharedRef()
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

void FPMSEditor::OnTryOpenSubGraph(UEdGraphNode* InNode)
{
    UE_LOG(LogTemp,Log,TEXT("TryOpen %s"),ToCStr(InNode->GetName()));
    if(UPMSEdSubGraphNode* Temp = Cast<UPMSEdSubGraphNode>(InNode)){
    	if(!Temp->SubGraph){
    		UPMSEdGraph* NewSubGraph = NewObject<UPMSEdGraph>(PMSGraphAsset,UPMSEdGraph::StaticClass());    	     
    	    NewSubGraph->bAllowDeletion = false;         
    	    NewSubGraph->Schema = UPMSEdGraphSchema::StaticClass();
    	    
    	    Temp->SubGraph = NewSubGraph;
    	    NewSubGraph->ParentNode = Temp;
    	    InNode->GetGraph()->SubGraphs.Add(NewSubGraph);
    	}
        PMSGraph = Temp->SubGraph;
        UpdateEditorByGraph(Temp->SubGraph);
    }
}

void AddSubMenuRecursively(FPMSEdGraphSchemaAction_ShelfToolSubMenu* SubMenu, FMenuBuilder& MenuBuilder)
{
	MenuBuilder.SetStyle(&FPMSEditorStyle::Get(),"PMSMenu");
	for(auto Child :SubMenu->Children)
	{
		// TSharedPtr<FEdGraphSchemaAction> Test1 = MakeShareable(new FPMSEdGraphSchemaAction_ShelfToolSubMenu);
		// TSharedPtr<FPMSEdGraphSchemaAction_ShelfToolSubMenu> Test2 = Test1;
		
		// bool bIsSubMenu = Child.Value->IsA(FPMSEdGraphSchemaAction_ShelfToolSubMenu::StaticStruct()->GetFName());
		// bool bIsShelfTool = Child.Value->IsA(FPMSEdGraphSchemaAction_ShelfTool::StaticStruct()->GetFName());

		bool bIsSubMenu = Child.Value->IsA(FName("FPMSEdGraphSchemaAction_ShelfToolSubMenu"));
		bool bIsShelfTool = Child.Value->IsA(FName("FPMSEdGraphSchemaAction_ShelfTool"));
		
		FPMSEdGraphSchemaAction_ShelfToolSubMenu* NextSubMenu = static_cast<FPMSEdGraphSchemaAction_ShelfToolSubMenu*>(Child.Value);
		FPMSEdGraphSchemaAction_ShelfTool* Action = static_cast<FPMSEdGraphSchemaAction_ShelfTool*>(Child.Value);
		
		if(bIsShelfTool)
		{
			FUIAction NewAction;
			FName NodeName = FName("PMSEditor.NodeIcons."+Action->IconName);
			//Todo Check If Icon Exist
			FSlateIcon Icon = FSlateIcon(FPMSEditorStyle::GetStyleSetName(),"PMSEditor.NodeIcons.polyexpand2d");
			
			if(FPMSEditorStyle::Get().GetBrush(NodeName)!=FPMSEditorStyle::Get().GetDefaultBrush())
				Icon = FSlateIcon(FPMSEditorStyle::GetStyleSetName(),NodeName);
			MenuBuilder.AddMenuEntry(
				FText::FromString(Action->Label),
				FText::FromString("NoTip"),
				Icon,
				NewAction);
		}
		if(bIsSubMenu)
		{
			MenuBuilder.AddSubMenu(FText::FromString(Child.Key.ToString()),FText::FromString("NoTip"),FNewMenuDelegate::CreateLambda(
				[NextSubMenu](FMenuBuilder& MenuBuilder)
				{					
					AddSubMenuRecursively(NextSubMenu,MenuBuilder);
				}
			));
		}
	}
}

FActionMenuContent FPMSEditor::OnGetContextMenu(UEdGraph* InGraph, const FVector2D& NodePos, const TArray<UEdGraphPin*>& DragedPins, bool, SGraphEditor::FActionMenuClosed OnClosedHandle)
{
	
	// FMenuBuilder MenuBuilder(true);
	// UToolMenu有Context，且可通过当前Context类型动态修改其中内容
	//InMenu->FindContext<UGraphNodeContextMenuContext>();
	
	// UToolMenu* Test;
	// FToolMenuSection& Section = Test->AddSection();
	// Section.AddSubMenu()
	// Section.AddEntry();

	FMenuBuilder MenuBuilder(true, MakeShareable(new FUICommandList));
	const UPMSEdGraphSchema* PMSSchema = Cast<UPMSEdGraphSchema>(InGraph->GetSchema());
	if(PMSSchema)
	{
		UPMSEdGraphSchema::InitPMSToolShelfLib();
		AddSubMenuRecursively(&(PMSSchema->PMSToolShelfLib),MenuBuilder);
	}
	
	// MenuBuilder.AddSearchWidget();
	// FName Customization = MenuBuilder.GetCustomization().GetCustomizationName();
	// UE_LOG(LogTemp,Log,TEXT("Custom Name %s"),*(Customization.ToString()));
	
	// MenuBuilder.SetStyle(&FEditorStyle::Get(),"LegacyViewportMenu");
	TSharedRef<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	
	return FActionMenuContent( MenuWidget );
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

void FPMSEditor::UpdateEditorByGraph(UPMSEdGraph* InGraph)
{
    if (InGraph != nullptr) {
        GraphToShow = InGraph;
        InArgs.GraphToEdit(InGraph);
        MakeTDecl<SGraphEditor>( "SGraphEditor", __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs() ) . Expose( GraphEditorViewport ) <<= TYPENAME_OUTSIDE_TEMPLATE InArgs;
        
        //EdGraphEditor
        SGraphEditorImpl* Implementation = (SGraphEditorImpl*)(((SGraphEditorPublic*)GraphEditorViewport.Get())->Implementation.Get());
        SGraphEditorImplPublic* ImplementationPublic = (SGraphEditorImplPublic*)Implementation;
        
        //((SGraphPanelFriend*)ImplementationPublic->GraphPanel.Get())->OnGetContextMenuFor;
        
        SPMSGraphPanel::FArguments NewGraphPanelArgs = SPMSGraphPanel::FArguments()
        .GraphObj(InGraph)
        .GraphObjToDiff(InArgs._GraphToDiff)
        //.OnGetContextMenuFor( ImplementationPublic, &SGraphEditorImplPublic::GraphEd_OnGetContextMenuFor )
        .OnSelectionChanged( InArgs._GraphEvents.OnSelectionChanged )
        .OnNodeDoubleClicked( InArgs._GraphEvents.OnNodeDoubleClicked )
        .IsEditable_Lambda( [this](){return InArgs._IsEditable.Get();} )
        .DisplayAsReadOnly_Lambda( [this](){return InArgs._DisplayAsReadOnly.Get();} )
        .OnDropActor( InArgs._GraphEvents.OnDropActor )
        .OnDropStreamingLevel( InArgs._GraphEvents.OnDropStreamingLevel )
        .OnVerifyTextCommit( InArgs._GraphEvents.OnVerifyTextCommit )
        .OnTextCommitted( InArgs._GraphEvents.OnTextCommitted )
        .OnSpawnNodeByShortcut( InArgs._GraphEvents.OnSpawnNodeByShortcut )
        //.OnUpdateGraphPanel( this, &SGraphEditorImpl::GraphEd_OnPanelUpdated )
        .OnDisallowedPinConnection( InArgs._GraphEvents.OnDisallowedPinConnection )
        .ShowGraphStateOverlay(InArgs._ShowGraphStateOverlay)
        .OnDoubleClicked(InArgs._GraphEvents.OnDoubleClicked);
        NewGraphPanelArgs._OnGetContextMenuFor = ((SGraphPanelPublic*)ImplementationPublic->GraphPanel.Get())->OnGetContextMenuFor;

    	// ((SGraphPanelPublic*)ImplementationPublic->GraphPanel.Get())->OnGetContextMenuFor.
    	
        MakeTDecl<SPMSGraphPanel>( "SPMSGraphPanel", __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs() ) . Expose( ImplementationPublic->GraphPanel ) <<= TYPENAME_OUTSIDE_TEMPLATE NewGraphPanelArgs;
        ImplementationPublic->GraphPanel->RestoreViewSettings(FVector2D::ZeroVector, -1);
        ImplementationPublic->GraphPanelSlot->AttachWidget(ImplementationPublic->GraphPanel.ToSharedRef());

        /*Rebuild GraphEditor*/
        GraphEditor->ClearChildren();

        GraphEditor->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Top)
        .AutoHeight()
        [
            SAssignNew(HirechyNavigation, SHorizontalBox)
        ];
        
        UpdateHirechyNavigation(InGraph);
        GraphEditor->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        .FillHeight(10000)
        [
            GraphEditorViewport.ToSharedRef()
        ];
    }
}

void FPMSEditor::UpdateHirechyNavigation(UPMSEdGraph* InGraph)
{
    if(HirechyNavigation)
    {
        HirechyNavigation->ClearChildren();

        TArray<UPMSEdSubGraphNode*> ButtonNodes;
        UPMSEdGraph* TempGraph = InGraph;
        while(TempGraph->ParentNode)
        {
            ButtonNodes.Add(Cast<UPMSEdSubGraphNode>(TempGraph->ParentNode));
            TempGraph = Cast<UPMSEdGraph>(Cast<UPMSEdSubGraphNode>(TempGraph->ParentNode)->GetGraph());
        }

        //Todo 将FAppStyle里注册的ButtonStyle移植到PMSEditorStyle里
        // auto RootButton = SNew(SButton)
        //         .ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
        //         .ContentPadding(FMargin(0.f,0.f,0.f,0.f))
        //         // .ForegroundColor(FSlateColor::UseForeground())
        //         // .ContentPadding(0.f)
        //         // .ButtonColorAndOpacity(FSlateColor(FLinearColor(0,0,0,0)))
        //         .Text(FText::FromString("Geo"))
        //         .OnClicked_Lambda([this,TempGraph]()
        //         {
        //             UpdateEditorByGraph(TempGraph);
        //             return FReply::Handled();
        //         });
        // RootButton->SetBorderImage(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Display"));

        // UE_LOG(LogTemp,Log,TEXT("Size = %s"),*(HirechyNavigation->GetDesiredSize().ToString()));
        
        HirechyNavigation->AddSlot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(SButton)
            .ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
            .ContentPadding(FMargin(0.f,0.f,0.f,0.f))
            .HAlign(HAlign_Left)
            // .ForegroundColor(FSlateColor::UseForeground())
            // .ContentPadding(0.f)
            // .ButtonColorAndOpacity(FSlateColor::UseForeground())
            .Text(FText::FromString("Geo"))
            .OnClicked_Lambda([this,TempGraph]()
            {
                if(this->GraphToShow!=TempGraph)
                UpdateEditorByGraph(TempGraph);
                return FReply::Handled();
            })
        ];
		
        for (int i = ButtonNodes.Num()-1; i>=0 ; i--)
        {
            UPMSEdSubGraphNode* Node = ButtonNodes[i];
            HirechyNavigation->AddSlot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FAppStyle::Get().GetWidgetStyle<FButtonStyle>("SimpleButton"))
                .ContentPadding(FMargin(10.f,0.f,0.f,0.f))
                .Text(FText::FromString(Node->NodeLabel.ToString()))
                .HAlign(HAlign_Left)
                .OnClicked_Lambda([this,Node]()
                {
                    if(this->GraphToShow!=Node->SubGraph)
                    UpdateEditorByGraph(Node->SubGraph);
                    return FReply::Handled();
                })
            ];
        }
        
        // UE_LOG(LogTemp,Log,TEXT("Size = %s"),*(HirechyNavigation->GetDesiredSize().ToString()));
    }    
}

/*
FActionMenuContent FPMSEditor::GraphEd_OnGetContextMenuFor(const FGraphContextMenuArguments& SpawnInfo)
{
	FActionMenuContent Result;

	if (GraphToShow != NULL)
	{
		Result = FActionMenuContent( SNew(STextBlock) .Text( NSLOCTEXT("GraphEditor", "NoNodes", "No Nodes") ) );

		const UEdGraphSchema* Schema = GraphToShow->GetSchema();
		check(Schema);
		
		//Todo 将此二者转为类变量，查看SGraphEditorImpl中的引用
		FEdGraphPinReference  GraphPinForMenu;
		TWeakObjectPtr<UEdGraphNode> GraphNodeForMenu;
		
		GraphPinForMenu.SetPin(SpawnInfo.GraphPin);
		GraphNodeForMenu = SpawnInfo.GraphNode;


		//若在Node或Pin上
		if ((SpawnInfo.GraphPin != NULL) || (SpawnInfo.GraphNode != NULL))
		{
			// Get all menu extenders for this context menu from the graph editor module
			FGraphEditorModule& GraphEditorModule = FModuleManager::GetModuleChecked<FGraphEditorModule>( TEXT("GraphEditor") );
			TArray<FGraphEditorModule::FGraphEditorMenuExtender_SelectedNode> MenuExtenderDelegates = GraphEditorModule.GetAllGraphEditorContextMenuExtender();

			TArray<TSharedPtr<FExtender>> Extenders;
			for (int32 i = 0; i < MenuExtenderDelegates.Num(); ++i)
			{
				if (MenuExtenderDelegates[i].IsBound())
				{
					Extenders.Add(MenuExtenderDelegates[i].Execute(this->Commands.ToSharedRef(), EdGraphObj, SpawnInfo.GraphNode, SpawnInfo.GraphPin, !IsEditable.Get()));
				}
			}
			TSharedPtr<FExtender> MenuExtender = FExtender::Combine(Extenders);

			if (OnCreateNodeOrPinMenu.IsBound())
			{
				// Show the menu for the pin or node under the cursor
				const bool bShouldCloseAfterAction = true;
				FMenuBuilder MenuBuilder( bShouldCloseAfterAction, this->Commands, MenuExtender );

				Result = OnCreateNodeOrPinMenu.Execute(EdGraphObj, SpawnInfo.GraphNode, SpawnInfo.GraphPin, &MenuBuilder, !IsEditable.Get());
			}
			else
			{
				UGraphNodeContextMenuContext* ContextObject = NewObject<UGraphNodeContextMenuContext>();
				ContextObject->Init(EdGraphObj, SpawnInfo.GraphNode, SpawnInfo.GraphPin, !IsEditable.Get());

				FToolMenuContext Context(this->Commands, MenuExtender, ContextObject);

				UAssetEditorToolkitMenuContext* ToolkitMenuContext = NewObject<UAssetEditorToolkitMenuContext>();
				ToolkitMenuContext->Toolkit = AssetEditorToolkit;
				Context.AddObject(ToolkitMenuContext);

				if (TSharedPtr<FAssetEditorToolkit> SharedToolKit = AssetEditorToolkit.Pin())
				{
					SharedToolKit->InitToolMenuContext(Context);
				}

				// Need to additionally pass through the asset toolkit to hook up those commands?

				UToolMenus* ToolMenus = UToolMenus::Get();
				UToolMenu* GeneratedMenu = GenerateContextMenu(Schema, Context);
				Result = FActionMenuContent(ToolMenus->GenerateWidget(GeneratedMenu));
			}
		}
		
		else if (IsEditable.Get())
		{
			if (EdGraphObj->GetSchema() != NULL )
			{
				if(OnCreateActionMenu.IsBound())
				{
					Result = OnCreateActionMenu.Execute(
						EdGraphObj, 
						SpawnInfo.NodeAddPosition,
						SpawnInfo.DragFromPins,
						bAutoExpandActionMenu, 
						SGraphEditor::FActionMenuClosed::CreateSP(this, &SGraphEditorImpl::OnClosedActionMenu)
						);
				}
				else
				{
					TSharedRef<SGraphEditorActionMenu> Menu =	
						SNew(SGraphEditorActionMenu)
						.GraphObj( EdGraphObj )
						.NewNodePosition(SpawnInfo.NodeAddPosition)
						.DraggedFromPins(SpawnInfo.DragFromPins)
						.AutoExpandActionMenu(bAutoExpandActionMenu)
						.OnClosedCallback( SGraphEditor::FActionMenuClosed::CreateSP(this, &SGraphEditorImpl::OnClosedActionMenu)
						);

					Result = FActionMenuContent( Menu, Menu->GetFilterTextBox() );
				}

				if (SpawnInfo.DragFromPins.Num() > 0)
				{
					GraphPanel->PreservePinPreviewUntilForced();
				}
			}
		}
		else
		{
			Result = FActionMenuContent( SNew(STextBlock)  .Text( NSLOCTEXT("GraphEditor", "CannotCreateWhileDebugging", "Cannot create new nodes in a read only graph") ) );
		}
	}
	else
	{
		Result = FActionMenuContent( SNew(STextBlock) .Text( NSLOCTEXT("GraphEditor", "GraphObjectIsNull", "Graph Object is Null") ) );
	}

	Result.OnMenuDismissed.AddLambda([this]()
	{
		bResetMenuContext = true;
	});

	return Result;
}

*/


#undef LOCTEXT_NAMESPACE

// typedef TDelegate<void(class UEdGraphNode*)> FSingleNodeEvent;;
// typedef TDelegate<void(const TSharedRef<SGraphEditor>&)> FOnFocused;;