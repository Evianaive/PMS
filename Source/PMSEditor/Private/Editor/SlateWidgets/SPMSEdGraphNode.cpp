#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

#include "Editor/PMSEdGraphNode.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Brushes/SlateImageBrush.h"

#include "Widgets/Layout/SScrollBox.h"

#include "HAL/FileManager.h"

//#include "Widgets/Input/"
#include "SGraphPin.h"
#include "Slate.h"
#include "Editor/SlateWidgets/SPMSEdGraphPin.h"

//Used in SetOwner
#include "GeomTools.h"
#include "MeshDescription.h"

#include "SGraphPanel.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEditorSettings.h"
#include "Editor/PMSEdSubGraphNode.h"
#include "Editor/SlateWidgets/S2DMeshWidget.h"
#include "Editor/SlateWidgets/SNodeFlagCheckBox.h"
#include "Editor/Style/PMSEditorStyle.h"

#define insert 1

#if insert
#define INSERT_BLOCK\
			+ SHorizontalBox::Slot()\
			.AutoWidth()\
			.VAlign(VAlign_Fill)\
			.HAlign(HAlign_Fill)\
			.Padding(FMargin(0.0f,0.0f))\
			[\
				SNew(SBorder)\
				.BorderImage(new FSlateColorBrush(FLinearColor(0.3f,0.3f,0.3f)))\
				.ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))\
				.Padding(FMargin(1.0f,0.0f))\
			]
#endif


//Todo ??InArgs????????Щ????
void SPMSEdGraphNode::Construct(const FArguments& InArgs, UPMSEdGraphNode* InNode)
{
	GraphNode = InNode;
    IconName = &(InNode->IconName);
    NodeMargin = &(InNode->Margin);
    NodeColor = &(InNode->Color);
	NodePadding = &(InNode->NodePadding);
	NodeSize = &(InNode->NodeSize);
	UpdateGraphNode();
}
/*
auto NodeIcon = [](FString NodeName, FVector2D NodeMargin) -> TSharedRef<SImage>
{
	//FString SearchDir = FPaths::ProjectPluginsDir().Append("HoudiniEdGraph/Resources/SOP/");
	//TArray<FString> FoundIcons;
	//IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDir, TEXT("*.svg"), true, false);
	FString SopDir = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/Icons/");
	FString NodeIconPath = SopDir.Append(NodeName);
	const FVector2D IconSize(48.f, 48.f);
	//UE_LOG(LogTemp, Log, TEXT("%s"), *NodeIconPath);
    
	//return SNew(SImage)
	//	.Image(new FSlateVectorImageBrush(NodeIconPath, IconSize, FLinearColor(1, 1, 1, 1)));

    FSlateVectorImageBrush* VectorImageBrush1 = new FSlateVectorImageBrush(NodeIconPath, NodeMargin);
    return SNew(SImage)
		//.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft)
		//.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,-60.0f)))
		//.FlipForRightToLeftFlowDirection(true)
        .Image(VectorImageBrush1);
};
*/


void SPMSEdGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	TopNodeBox.Reset();
	BottomNodeBox.Reset();
	
	int32 FontSize = 30;
	FSlateFontInfo FontDefault = FCoreStyle::GetDefaultFontStyle("Regular", FontSize);
	
	FString NodeNameFString = IconName->LeftChop(4);
	FName NodeName = FName("PMSEditor.NodeIcons."+NodeNameFString);
	FVector2D PinHorizontalBoxSize = FVector2D(NodeSize->X*0.6,NodeSize->Y*0.25);
	
	if(GetDefault<UPMSEditorSettings>()->bNodeContextMode)
	{
		GetOrAddSlot(ENodeZone::Center)
		.SlotSize(*NodeSize)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			//.BorderBackgroundColor(*NodeColor)
			.BorderBackgroundColor_Lambda([this]()
			{
				//Todo Hover Color Change
				return FSlateColor(*this->NodeColor);
			})
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.Padding(FMargin(1.0f,1.0f))
			.Clipping(EWidgetClipping::ClipToBoundsAlways)
			[
				SNew(SHorizontalBox)	                
	            + SHorizontalBox::Slot()
	            .AutoWidth()
	            .VAlign(VAlign_Fill)
	            .HAlign(HAlign_Fill)
				.Padding(FMargin(0.0f,0.0f))
	            [
	                SAssignNew(BypassFlag,SNodeFlagCheckBox)
	                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Bypass"))                
	                .IsChecked_Lambda([this]()->ECheckBoxState{return Cast<UPMSEdGraphNode>(this->GraphNode)->BypassState? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
	                .OnCheckStateChanged(this, &SPMSEdGraphNode::OnBypassChanged)
				]
	#if insert
				INSERT_BLOCK
	#endif
	            + SHorizontalBox::Slot()
	            .AutoWidth()
	            .VAlign(VAlign_Fill)
	            .HAlign(HAlign_Fill)
				.Padding(FMargin(0.0f,0.0f))
	            [
	                SAssignNew(LockFlag,SNodeFlagCheckBox)
	                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Lock"))
	                .IsChecked_Lambda([this]()->ECheckBoxState{return Cast<UPMSEdGraphNode>(this->GraphNode)->LockState? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
	                .OnCheckStateChanged(this, &SPMSEdGraphNode::OnLockChanged)
	            ]
	#if insert
				INSERT_BLOCK
	#endif
				+ SHorizontalBox::Slot()
				.AutoWidth()
	            .VAlign(VAlign_Center)
	            .HAlign(HAlign_Center)
				.Padding(FMargin(*NodePadding))
				[
					SNew(SImage)
					.Image(FPMSEditorStyle::Get().GetBrush(NodeName))
					.DesiredSizeOverride(FVector2D(48.0f,48.0f))
					
				]
	#if insert
				INSERT_BLOCK
	#endif
	            + SHorizontalBox::Slot()
	            .AutoWidth()
	            .VAlign(VAlign_Fill)
	            .HAlign(HAlign_Fill)
				.Padding(FMargin(0.0f,0.0f))
	            [
	                SAssignNew(TemplateFlag,SNodeFlagCheckBox)
	                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Template"))
	                .IsChecked_Lambda([this]()->ECheckBoxState{return Cast<UPMSEdGraphNode>(this->GraphNode)->TemplateState? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
	                .OnCheckStateChanged(this, &SPMSEdGraphNode::OnTemplateChanged)
				]
	#if insert
				INSERT_BLOCK
	#endif
	            + SHorizontalBox::Slot()
	            //.FillWidth(0.145f)
	            .AutoWidth()
	            .VAlign(VAlign_Fill)
	            .HAlign(HAlign_Fill)
				.Padding(FMargin(0.0f,0.0f))
	            [
	                SAssignNew(DisplayFlag,SNodeFlagCheckBox)
	                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Display"))
	                .IsChecked_Lambda([this]()->ECheckBoxState{return Cast<UPMSEdGraphNode>(this->GraphNode)->DisplayState? ECheckBoxState::Checked : ECheckBoxState::Unchecked;})
	                .OnCheckStateChanged( this, &SPMSEdGraphNode::OnDisplayChanged)
	            ]
			]
			
		];	
	}
	else
	{
		GetOrAddSlot(ENodeZone::Center)
		[
			SNew(S2DMeshWidget)
		]
		;
	}
	GetOrAddSlot(ENodeZone::Right)
	.SlotSize(PinHorizontalBoxSize)
	.SlotOffset(FVector2D((NodeSize->X-PinHorizontalBoxSize.X)/2,(+NodeSize->Y+5)))
	.HAlign(HAlign_Center)
	[
		SAssignNew(BottomNodeBox, SHorizontalBox)		
	];
	GetOrAddSlot(ENodeZone::Left)
	.SlotSize(PinHorizontalBoxSize)
	.SlotOffset(FVector2D((NodeSize->X-PinHorizontalBoxSize.X)/2,-(PinHorizontalBoxSize.Y+5)))
	.HAlign(HAlign_Center)
	[
		SAssignNew(TopNodeBox, SHorizontalBox)
	];
	GetOrAddSlot(ENodeZone::BottomCenter)
	.SlotOffset(FVector2D(NodeSize->X,(NodeSize->Y-FontSize*3.0f/2.0f)/2))
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SAssignNew(Label,SEditableText)
			.Text(this, &SPMSEdGraphNode::GetNodeLabel)
			.OnTextCommitted(this, &SPMSEdGraphNode::OnLabelTextCommitted)
			.Clipping(EWidgetClipping::Inherit)
			.Font(FontDefault)
			.ColorAndOpacity(FLinearColor(1.0f,1.0f,1.0f,0.8))
		]
	];
	
	CreatePinWidgets();
}
void SPMSEdGraphNode::SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel)
{
	check( !OwnerGraphPanelPtr.IsValid() );
	SetParentPanel(OwnerPanel);
	OwnerGraphPanelPtr = OwnerPanel;
	GraphNode->DEPRECATED_NodeWidget = SharedThis(this);

	//TODO: I am not sure this variable should be named like this
	/*Once we have an owner, and if hide Unused pins is enabled, we need to remake our pins to drop the hidden ones*/
	if(OwnerGraphPanelPtr.Pin()->GetPinVisibility() != SGraphEditor::Pin_Show 
		&& TopNodeBox.IsValid()
		&& BottomNodeBox.IsValid())
	{
		this->TopNodeBox->ClearChildren();
		this->BottomNodeBox->ClearChildren();
		CreatePinWidgets();
	}
}
void SPMSEdGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	//SGraphNode::AddPin(PinToAdd);
	
	//TODO This is for Horizontal Layout for Houdini Style
	PinToAdd->SetOwner(SharedThis(this));
	PinToAdd->SetColorAndOpacity(FLinearColor(0.9f,0.9f,0.9f,1.0f));
	PinToAdd->SetClipping(EWidgetClipping::ClipToBoundsAlways);

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}
	//TODO Need to figure out why Settings->GetInputPinPadding() can not be used
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		TopNodeBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(1.0f)
		.Padding(FMargin(1.0f,1.0f))
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
		{
		BottomNodeBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(1.0f)
		.Padding(FMargin(1.0f,1.0f))
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
		}
}

void SPMSEdGraphNode::CreatePinWidgets()
{
	UPMSEdGraphNode* PMSEdGraphNode = CastChecked<UPMSEdGraphNode>(GraphNode);
	for(auto Pin:PMSEdGraphNode->Pins)
	{
		if(!Pin->bHidden)
		{
			/*TODO SGraphPin ?????????????????????????????*/
			TSharedPtr<SGraphPin>NewPin = SNew(SPMSEdGraphPin, Pin)
			// .RenderTransform(FVector2D(0.0f,20.0f))
			;
			/*????????н?????*/
			//NewPin->SetIsEditable(IsEditable);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SPMSEdGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty) {
    if (!NodeFilter.Find(SharedThis(this)))
    {
        if (GraphNode && !RequiresSecondPassLayout())
        {
            NodeFilter.Add(SharedThis(this));
            GraphNode->Modify(bMarkDirty);
            GraphNode->NodePosX = NewPosition.X;
            GraphNode->NodePosY = NewPosition.Y;
        	//TSharedPtr<SGraphPanel> ParentPanel = GetOwnerPanel();
        	//TSharedPtr<SNodePanel> NodePanel = ParentPanel;
        	//SharedThis(SNode)
        	//TSharedPtr<SNodePanel> NodePanel = ParentPanelPtr.Pin();
			//NodePanel->NodeUnderMousePtr;
        	
        	//Cast<SNodePanel>(GetParentWidget());
        	//GetOwnerPanel()->NodeUnderMousePtr;
        	//if(ParentPanel->
        	// Cast<UPMSEdGraphNode>(GraphNode)->PMSSnapToGrid(128.0f,16.0f);
        	// UPMSEdGraph* Graph = Cast<UPMSEdGraph>(GraphNode->GetOuter());
        	// Graph->SelectNode = Cast<UPMSEdGraphNode>(GraphNode);
            // UpdateGraphNode();
        }
    }
}

void SPMSEdGraphNode::OnBypassChanged(ECheckBoxState InNewState)
{
	UPMSEdGraphNode* PMSGraphNode = Cast<UPMSEdGraphNode>(this->GraphNode);
	PMSGraphNode->BypassState = !PMSGraphNode->BypassState;
	if(InNewState==ECheckBoxState::Checked)
	{
		//PMSGraphNode->Color *= FLinearColor(0.4f,0.4f,0.4f,0.0f);
		PMSGraphNode->Color *= 0.2;
		PMSGraphNode->Color.A = 1.0f;
	}
	else
	{
		//PMSGraphNode->Color += FLinearColor(0.4f,0.4f,0.4f,0.0f);
		PMSGraphNode->Color *= 5.0f;
		PMSGraphNode->Color.A = 1.0f;
	}	
}

void SPMSEdGraphNode::OnLockChanged(ECheckBoxState InNewState)
{
	UPMSEdGraphNode* PMSGraphNode = Cast<UPMSEdGraphNode>(this->GraphNode);
	PMSGraphNode->LockState = !PMSGraphNode->LockState;
}

void SPMSEdGraphNode::OnTemplateChanged(ECheckBoxState InNewState)
{
	UPMSEdGraphNode* PMSGraphNode = Cast<UPMSEdGraphNode>(this->GraphNode);
	PMSGraphNode->TemplateState = !PMSGraphNode->TemplateState;
}

void SPMSEdGraphNode::OnDisplayChanged(ECheckBoxState InNewState)
{
	UPMSEdGraph* ParentGraph = Cast<UPMSEdGraph>(this->GraphNode->GetGraph());
	if(this->GraphNode!=ParentGraph->DisplayNode)
	{
		//TSharedPtr<SPMSEdGraphNode> CurDisplay = StaticCastSharedPtr<SPMSEdGraphNode>(Cast<UPMSEdGraphNode>(ParentGraph->DisplayNode)->SlateNode);
		
		Cast<UPMSEdGraphNode>(ParentGraph->DisplayNode)->DisplayState = false;
		Cast<UPMSEdGraphNode>(this->GraphNode)->DisplayState = true;
		ParentGraph->DisplayNode = this->GraphNode;
	}
	// else
	// {
	// 	this->DisplayFlag->SetIsChecked(ECheckBoxState::Checked);
	// }
	//this->TemplateFlag->SetIsChecked(ECheckBoxState::Unchecked);
}

FText SPMSEdGraphNode::GetNodeLabel() const
{
	return Cast<UPMSEdGraphNode>(GraphNode)->NodeLabel;
}

UPMSEdGraphNode* SPMSEdGraphNode::GetPMSNodeObj()
{
	return Cast<UPMSEdGraphNode>(GraphNode);
}

void SPMSEdGraphNode::OnLabelTextCommitted(const FText& NewLabel, ETextCommit::Type CommitInfo)
{
	Cast<UPMSEdGraphNode>(GraphNode)->NodeLabel = NewLabel;
}

void SPMSEdGraphNode::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SGraphNode::OnMouseEnter(MyGeometry, MouseEvent);
}

FReply SPMSEdGraphNode::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	// if(UPMSEdSubGraphNode* Temp = Cast<UPMSEdSubGraphNode>(GraphNode)){
	// 	if(Temp->SubGraph){	
	// 		// Temp->GetGraph()
	// 	}
	// 	else{
	// 		//Todo 完成创建的部分
	// 		NewObject<UPMSEdGraph>();
	// 	}
	// }
	return SGraphNode::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	// Todo 查看一下OnDoubleClick的使用情况
	
	// if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	// {
	// 	OnDoubleClick.ExecuteIfBound(GraphNode);
	// 	return FReply::Handled();
	// }
	// return FReply::Unhandled();
}
