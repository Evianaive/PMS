// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SGraphNode.h"

/**
 * 
 */

class UPMSEdGraphNode;
struct FLinearColor;

class SPMSEdGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SPMSEdGraphNode) {}

	SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs, UPMSEdGraphNode* InNode);

	// SGraphNode implementation
	virtual void UpdateGraphNode() override;
	// End SGraphNode implementation
	
	virtual void CreatePinWidgets() override;
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;
	
	//SetOwner is used in AddPin，These two function are related to LeftNodeBox and RightNodeBox
	virtual void SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel) override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	
	FVector2D* NodeMargin;
	FVector2D* Pd;
	FString* IconName;
	FLinearColor* NodeColor;
	//为了实现上下的节点输入Box
	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SHorizontalBox> BottomNodeBox;
};