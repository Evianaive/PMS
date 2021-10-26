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


//���԰�����ͼ�������Ǹ�search svg�Ķ���������ߣ�Ȼ���ϵ������еķ�ʽ
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
	const FVector2D IconSize(32.f, 32.f);
	UE_LOG(LogTemp, Log, TEXT("%s"), *NodeIconPath);
    
	//return SNew(SImage)
	//	.Image(new FSlateVectorImageBrush(NodeIconPath, IconSize, FLinearColor(1, 1, 1, 1)));

    FSlateVectorImageBrush* VectorImageBrush1 = new FSlateVectorImageBrush(NodeIconPath, NodeMargin);
    return SNew(SImage)
        .Image(VectorImageBrush1)
		//.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft);
};

void SPMSEdGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	TopNodeBox.Reset();
	BottomNodeBox.Reset();
	
	GetOrAddSlot(ENodeZone::Center)
	[
		 SNew(SBorder)
		 .Padding(FMargin(0.f,0.f))
		 //.ColorAndOpacity(FLinearColor(0,0,0,0))
		 .BorderBackgroundColor(*NodeColor)
		 //.FlipForRightToLeftFlowDirection(true)
		 .BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		// SNew(SHorizontalBox)
		// + SHorizontalBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Fill)
			.FillHeight(1.0f)
			[
				SAssignNew(TopNodeBox, SHorizontalBox).RenderTransform(FSlateRenderTransform(FVector2D(0.0f,0.0f)))
			]
			+ SVerticalBox::Slot()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
            .AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
				.BorderBackgroundColor(*NodeColor)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.Padding(FMargin(0.5f,0.5f))
				[
					SNew(SHorizontalBox)
	                +SHorizontalBox::Slot()
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
	                .FillWidth(1.0f)
					.Padding(FMargin(0.5f,0.5f))
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
					+ SHorizontalBox::Slot()
	                .FillWidth(1.0f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.5f,0.5f))
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
					+ SHorizontalBox::Slot()
					.AutoWidth()
	                .VAlign(VAlign_Center)
	                .HAlign(HAlign_Center)
					.Padding(FMargin(*Pd))
					[
						NodeIcon(*IconName,*NodeMargin)
					]
	                + SHorizontalBox::Slot()
					.AutoWidth()
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.5f,0.5f))
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
	                + SHorizontalBox::Slot()
	                .FillWidth(1.0f)
	                .VAlign(VAlign_Fill)
	                .HAlign(HAlign_Fill)
					.Padding(FMargin(0.5f,0.5f))
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
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Fill)
			.AutoHeight()
			[
				SAssignNew(BottomNodeBox, SHorizontalBox)
				.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,20.0f)))
			]
		]
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
			/*TODO SGraphPin Ҫ��Ϊ�Լ�ʵ�ֵ�������ʵ�ֲ�һ�������*/
			TSharedPtr<SGraphPin>NewPin = SNew(SPMSEdGraphPin, Pin);
			/*�ɷ�༭ֱ�Ӽ̳нڵ�ɷ�༭*/
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