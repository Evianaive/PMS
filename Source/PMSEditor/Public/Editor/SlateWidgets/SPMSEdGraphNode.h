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
class SNodeFlagCheckBox;

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

	void OnBypassChanged(ECheckBoxState InNewState);
	void OnLockChanged(ECheckBoxState InNewState);
	void OnTemplateChanged(ECheckBoxState InNewState);
	void OnDisplayChanged(ECheckBoxState InNewState);

	FText GetNodeLabel() const;
	UPMSEdGraphNode* GetPMSNodeObj();
	void OnLabelTextCommitted(const FText& NewLabel, ETextCommit::Type CommitInfo);

	//TODO Parameter related with NodeShape, this should be moved to SLATE_BEGIN_ARGS
	FVector2D* NodeMargin;
	FVector2D* NodePadding;
	FVector2D* NodeSize;
	
	FString* IconName;
	FLinearColor* NodeColor;
	//为了实现上下的节点输入Box，原先为LeftNodeBox和RightNodeBox
	TSharedPtr<SHorizontalBox> TopNodeBox;
	TSharedPtr<SHorizontalBox> BottomNodeBox;

	//为了实现侧边浮动可编辑的Label
	TSharedPtr<SEditableText> Label;

	//NodeFlags
	TSharedPtr<SNodeFlagCheckBox> BypassFlag;
	TSharedPtr<SNodeFlagCheckBox> LockFlag;
	TSharedPtr<SNodeFlagCheckBox> TemplateFlag;
	TSharedPtr<SNodeFlagCheckBox> DisplayFlag;	
};