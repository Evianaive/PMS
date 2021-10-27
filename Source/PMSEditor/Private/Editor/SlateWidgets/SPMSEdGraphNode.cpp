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
#include "SGraphPanel.h"

#define insert 0
//可以把所有图标用它那个search svg的东西画在左边，然后拖到场景中的方式
void SPMSEdGraphNode::Construct(const FArguments& InArgs, UPMSEdGraphNode* InNode)
{
	GraphNode = InNode;
    IconName = &(InNode->IconName);
    NodeMargin = &(InNode->Margin);
    NodeColor = &(InNode->Color);
	Pd = &(InNode->Pd);
	UpdateGraphNode();
}

auto LeftRightLabel = [](const FName& InIconName = FName(), const FText& InLabel = FText::GetEmpty(), const FName& InTextStyle = TEXT("ButtonText")) -> TSharedRef<SWidget>
{
	TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);
	float Space = InIconName.IsNone() ? 0.0f : 8.f;

	if (!InIconName.IsNone())
	{
		HBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(
				FAppStyle::Get().GetBrush(InIconName)
				//new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"), 1.f)
			)
			];
	}

	if (!InLabel.IsEmpty())
	{
		HBox->AddSlot()
			.VAlign(VAlign_Center)
			.Padding(Space, 0.5f, 0.f, 0.f)  // Compensate down for the baseline since we're using all caps
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&FAppStyle::Get().GetWidgetStyle< FTextBlockStyle >(InTextStyle))
			.Justification(ETextJustify::Center)
			.Text(InLabel)
			];
	}

	return SNew(SBox).HeightOverride(16.f)[HBox.ToSharedRef()];
};

auto NodeIcon = [](FString NodeName, FVector2D NodeMargin) -> TSharedRef<SImage>
{
	/*FString SearchDir = FPaths::ProjectPluginsDir().Append("HoudiniEdGraph/Resources/SOP/");
	TArray<FString> FoundIcons;
	IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDir, TEXT("*.svg"), true, false);*/
	FString SopDir = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/SOP/");
	FString NodeIconPath = SopDir.Append(NodeName);
	const FVector2D IconSize(48.f, 48.f);
	UE_LOG(LogTemp, Log, TEXT("%s"), *NodeIconPath);
    
	//return SNew(SImage)
	//	.Image(new FSlateVectorImageBrush(NodeIconPath, IconSize, FLinearColor(1, 1, 1, 1)));

    FSlateVectorImageBrush* VectorImageBrush1 = new FSlateVectorImageBrush(NodeIconPath, NodeMargin);
    return SNew(SImage)
		//.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft)
        .Image(VectorImageBrush1);
};

void SPMSEdGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	TopNodeBox.Reset();
	BottomNodeBox.Reset();

	FSlateFontInfo Font = FSlateFontInfo();
	Font.Size = 100;
	GetOrAddSlot(ENodeZone::Center)
	//TODO 可否通过设置SlotSize来限制节点大小
	//.SlotSize(FVector2D(200.f,60.f))
	[
		 SNew(SBorder)
		 .Padding(FMargin(0.5f,0.5f))
		 //.ColorAndOpacity(FLinearColor(0,0,0,0))
		 .BorderBackgroundColor(*NodeColor)
		 //.FlipForRightToLeftFlowDirection(true)
		 .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		// SNew(SHorizontalBox)
		// + SHorizontalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
				//.BorderBackgroundColor(*NodeColor-FLinearColor(0.5f,0.5f,0.5f,0.0f))
				.BorderBackgroundColor(*NodeColor)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.Padding(FMargin(0.0f,0.0f))
				[
					SNew(SHorizontalBox)	                
	                + SHorizontalBox::Slot()
	                .FillWidth(0.145f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
                    [
                        SNew(SCheckBox)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
		                //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
		                //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
		                .CheckedImage(new FSlateColorBrush(FLinearColor(300.f,.65f,1.f).HSVToLinearRGB()))
		                .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
		                .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
		                .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
		                .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
						//.Padding(FMargin(0.0f,0.0f))
					]
#if insert
					+ SHorizontalBox::Slot()
					.FillWidth(0.005f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
					[
						SNew(SImage).ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))
					]
#endif
	                + SHorizontalBox::Slot()
	                .FillWidth(0.145f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
	                [
	                    SNew(SCheckBox)
	                    .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
	                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
	                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
	                    .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
	                    .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
	                    .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
	                    .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
	                    .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
	                    //.Padding(FMargin(0.0f,0.0f))
                    ]
#if insert
					+ SHorizontalBox::Slot()
					.FillWidth(0.005f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
					[
						SNew(SImage).ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))
					]
#endif
					+ SHorizontalBox::Slot()
					//.AutoWidth()
					.FillWidth(0.4f)
	                .VAlign(VAlign_Center)
	                .HAlign(HAlign_Center)
					.Padding(FMargin(*Pd))
					//.Padding(FMargin(0.5f,0.5f))
					[
						NodeIcon(*IconName,*NodeMargin)
					]
#if insert
					+ SHorizontalBox::Slot()
					.FillWidth(0.005f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
					[
						SNew(SImage).ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))
					]
#endif
	                + SHorizontalBox::Slot()
	                .FillWidth(0.145f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
                    [
                        SNew(SCheckBox)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
		                //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
		                //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
		                .CheckedImage(new FSlateColorBrush(FLinearColor(300.f,.65f,1.f).HSVToLinearRGB()))
		                .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
		                .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
		                .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
		                .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
						//.Padding(FMargin(0.0f,0.0f))
					]
#if insert
					+ SHorizontalBox::Slot()
					.FillWidth(0.005f)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
					[
						SNew(SImage).ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))
					]
#endif
	                + SHorizontalBox::Slot()
	                .FillWidth(0.145f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.0f,0.5f))
	                [
	                    SNew(SCheckBox)
	                    .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
	                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
	                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
	                    .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
	                    .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
	                    .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
	                    .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
	                    .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
	                    //.Padding(FMargin(0.0f,0.0f))
	                ]
				]
				
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(BottomNodeBox, SHorizontalBox)
				.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,20.0f)))
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Fill)
			[
				SAssignNew(TopNodeBox, SHorizontalBox)
				.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,-20.0f)))
			]
		]
	];

	/*TODO 确认是否可以将TopNodeBox移动到其他ENodeZone之中
	 *猜测如果是没有被占用的ENodeZone就可以使用，否则得在类里面override掉
	 *可以直接使用SlotOffset来移动Slot
	 *Clip的设置
	*/
	GetOrAddSlot(ENodeZone::Right)
	.SlotSize(FVector2D(500.f,100.f))
	[
		SNew(STextBlock)
		.Text(FText::FromString("TestNodeName"))
		.RenderTransform(FSlateRenderTransform(FVector2D(250.f,0.0f)))
		.Font(Font)
	];
	/*GetOrAddSlot(ENodeZone::Center)
	[

		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(FMargin(*NodeMargin))
		.BorderBackgroundColor(*NodeColor)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SAssignNew(TopNodeBox, SHorizontalBox)
			]
			+ SVerticalBox::Slot()
			//.AutoHeight()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			[
				SNew(SHorizontalBox)
				/* + SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
					[
						LeftRightLabel("Icons.box-perspective")
					]
				]
                #1#
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Fill)
                .HAlign(HAlign_Fill)
                //.AutoWidth()
                [
                    SNew(SCheckBox)
                    .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                    .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
                    .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                    .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                    .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                    .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                ]
				+ SHorizontalBox::Slot()
				.AutoWidth()
                .VAlign(VAlign_Fill)
				[
					SNew(SCheckBox)
					.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
					//.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
					//.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
					.CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
					.CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
					.CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
					.UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
					.UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
                .VAlign(VAlign_Center)
                
				[
					NodeIcon(*IconName)
				]
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SCheckBox)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
		                //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
		                //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
		                .CheckedImage(new FSlateColorBrush(FLinearColor(300.f,.65f,1.f).HSVToLinearRGB()))
		                .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
		                .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
		                .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
		                .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
					]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                    .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                    .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
                    .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                    .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                    .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                    .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                ]            
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SAssignNew(BottomNodeBox, SHorizontalBox)
			]
		]
	];*/
	
	CreatePinWidgets();
}
void SPMSEdGraphNode::SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel)
{
	//TODO: I am not sure this variable should be named like this
	check( !OwnerGraphPanelPtr.IsValid() );
	SetParentPanel(OwnerPanel);
	OwnerGraphPanelPtr = OwnerPanel;
	GraphNode->DEPRECATED_NodeWidget = SharedThis(this);

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
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
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
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
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
			/*TODO SGraphPin 要改为自己实现的子类来实现不一样的外观*/
			TSharedPtr<SGraphPin>NewPin = SNew(SPMSEdGraphPin, Pin);
			/*可否编辑直接继承节点可否编辑*/
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
            
            UpdateGraphNode();
        }
    }
}