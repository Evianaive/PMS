// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/SPMSEdGraphPin.h"

#include "SlateOptMacros.h"

#include "SPinTypeSelector.h"
#include "Widgets/Layout/SWrapBox.h"
#include "SLevelOfDetailBranchNode.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPMSEdGraphPin::Construct(const FArguments& InArgs,UEdGraphPin* InPin)
{
	/*这几行主要是设置一下Cursor，check一下有没有节点，有没有Scheme*/
	bUsePinColorForText = InArgs._UsePinColorForText;
	this->SetCursor(EMouseCursor::Default);

	SetVisibility(MakeAttributeSP(this, &SPMSEdGraphPin::GetPinVisiblity));

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	checkf(
		Schema, 
		TEXT("Missing schema for pin: %s with outer: %s of type %s"), 
		*(GraphPinObj->GetName()),
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetName()) : TEXT("NULL OUTER"), 
		GraphPinObj->GetOuter() ? *(GraphPinObj->GetOuter()->GetClass()->GetName()) : TEXT("NULL OUTER")
	);

	/*调整Icon类型为Original，即不带箭头的类型，
	 *在SGraphPin的构造函数中有一句
	 *EBlueprintPinStyleType StyleType = GetDefault<UGraphEditorSettings>()->DataPinStyle
	 *这在SPMSEdGraphPin中还未实现
	 */
	
	CachedImg_Pin_Connected = FEditorStyle::GetBrush( "Graph.Pin.Connected" );
	CachedImg_Pin_Disconnected = FEditorStyle::GetBrush( "Graph.Pin.Disconnected" );
	
	/*输入类型是不是Input*/
	const bool bIsInput = (GetDirection() == EGPD_Input);
	
	// Create the pin icon widget
	TSharedRef<SWidget> PinWidgetRef = SPinTypeSelector::ConstructPinTypeImage(
		MakeAttributeSP(this, &SPMSEdGraphPin::GetPinIcon ),
		MakeAttributeSP(this, &SPMSEdGraphPin::GetPinColor),
		MakeAttributeSP(this, &SPMSEdGraphPin::GetSecondaryPinIcon),
		MakeAttributeSP(this, &SPMSEdGraphPin::GetSecondaryPinColor));
	PinImage = PinWidgetRef;

	/*Todo 此处可以通过更改函数GEtPMSPinCursor替换Pin Cursor图标，原先使用函数为GetPinCursor*/
	PinWidgetRef->SetCursor( 
		TAttribute<TOptional<EMouseCursor::Type> >::Create (
			TAttribute<TOptional<EMouseCursor::Type> >::FGetter::CreateRaw( this, &SPMSEdGraphPin::GetPMSPinCursor )
		)
	);

	// Create the pin indicator widget (used for watched values)
	static const FName NAME_NoBorder("NoBorder");
	TSharedRef<SWidget> PinStatusIndicator =
		SNew(SButton)
		.ButtonStyle(FEditorStyle::Get(), NAME_NoBorder)
		.Visibility(this, &SPMSEdGraphPin::GetPinStatusIconVisibility)
		.ContentPadding(0)
		.OnClicked(this, &SPMSEdGraphPin::ClickedOnPinStatusIcon)
		[
			SNew(SImage)
			.Image(this, &SPMSEdGraphPin::GetPinStatusIcon)
		];

	TSharedRef<SWidget> LabelWidget = GetLabelWidget(InArgs._PinLabelStyle);

	// Create the widget used for the pin body (status indicator, label, and value)
	LabelAndValue =
		SNew(SWrapBox)
		.PreferredSize(150.f);

	if (!bIsInput)
	{
		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				LabelWidget
			];
	}
	else
	{
		LabelAndValue->AddSlot()
			.VAlign(VAlign_Center)
			[
				LabelWidget
			];

		/*如果这是Input就要设置参数框
		ValueWidget = GetDefaultValueWidget();

		if (ValueWidget != SNullWidget::NullWidget)
		{
			TSharedPtr<SBox> ValueBox;
			LabelAndValue->AddSlot()
				.Padding(bIsInput ? FMargin(InArgs._SideToSideMargin, 0, 0, 0) : FMargin(0, 0, InArgs._SideToSideMargin, 0))
				.VAlign(VAlign_Center)
				[
					SAssignNew(ValueBox, SBox)
					.Padding(0.0f)
					[
						ValueWidget.ToSharedRef()
					]
				];

			if (!DoesWidgetHandleSettingEditingEnabled())
			{
				ValueBox->SetEnabled(TAttribute<bool>(this, &SGraphPin::IsEditingEnabled));
			}
		}
		*/
	}

	TSharedPtr<SHorizontalBox> PinContent;
	if (bIsInput)
	{
		// Input pin
		FullPinHorizontalRowWidget = PinContent = 
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Fill)
			.Padding(0, 0, InArgs._SideToSideMargin, 0)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					PinWidgetRef
				]	
			]
			// +SHorizontalBox::Slot()
			// .AutoWidth()
			// .VAlign(VAlign_Center)
			// [
			// 	LabelAndValue.ToSharedRef()
			// ]
		;
	}
	else
	{
		// Output pin
		FullPinHorizontalRowWidget = PinContent = SNew(SHorizontalBox)
			// +SHorizontalBox::Slot()
			// .AutoWidth()
			// .VAlign(VAlign_Center)
			// [
			// 	LabelAndValue.ToSharedRef()
			// ]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Fill)
			.Padding(InArgs._SideToSideMargin, 0, 0, 0)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					PinWidgetRef
				]				
			]
		;
	}

	// Set up a hover for pins that is tinted the color of the pin.
	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SPMSEdGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SPMSEdGraphPin::GetPinColor)
		.OnMouseButtonDown(this, &SPMSEdGraphPin::OnPinNameMouseDown)
		[
			SNew(SLevelOfDetailBranchNode)
			.UseLowDetailSlot(this, &SPMSEdGraphPin::UseLowDetailPinNames)
			.LowDetail()
			[
				//@TODO: Try creating a pin-colored line replacement that doesn't measure text / call delegates but still renders
				PinWidgetRef
			]
			.HighDetail()
			[
				//PinWidgetRef
				PinContent.ToSharedRef()
			]
		]
	);

	TSharedPtr<IToolTip> TooltipWidget = SNew(SToolTip)
		.Text(this, &SPMSEdGraphPin::GetTooltipText)
		.IsInteractive(this, &SPMSEdGraphPin::IsTooltipInteractive)
		.OnSetInteractiveWindowLocation(this, &SPMSEdGraphPin::OnSetInteractiveTooltipLocation);

	SetToolTip(TooltipWidget);
}

FSlateColor SPMSEdGraphPin::GetPinColor() const
{
	// UEdGraphPin* GraphPin = GetPinObj();
	// if (GraphPin && !GraphPin->IsPendingKill())
	// {
	// 	if (GraphPin->bIsDiffing)
	// 	{
	// 		return FSlateColor(FLinearColor(0.9f, 0.2f, 0.15f));
	// 	}
	// 	if (GraphPin->bOrphanedPin)
	// 	{
	// 		return FSlateColor(FLinearColor::Red);
	// 	}
	// 	if (const UEdGraphSchema* Schema = GraphPin->GetSchema())
	// 	{
	// 		if (!GetPinObj()->GetOwningNode()->IsNodeEnabled() || GetPinObj()->GetOwningNode()->IsDisplayAsDisabledForced() || !IsEditingEnabled() || GetPinObj()->GetOwningNode()->IsNodeUnrelated())
	// 		{
	// 			return Schema->GetPinTypeColor(GraphPin->PinType) * FLinearColor(1.0f, 1.0f, 1.0f, 0.5f);
	// 		}
	//
	// 		return Schema->GetPinTypeColor(GraphPin->PinType) * PinColorModifier;
	// 	}
	// }

	return FLinearColor::White;
}

const FSlateBrush* SPMSEdGraphPin::GetPinIcon() const
{
	return SGraphPin::GetPinIcon();
}

TOptional<EMouseCursor::Type> SPMSEdGraphPin::GetPMSPinCursor() const
{
	check(PinImage.IsValid());

	if (PinImage->IsHovered())
	{
		if (bIsMovingLinks)
		{
			return EMouseCursor::GrabHandClosed;
		}
		else
		{
			return EMouseCursor::Crosshairs;
		}
	}
	else
	{
		return EMouseCursor::Default;
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
