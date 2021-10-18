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
	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty = true) override;

	FVector2D* NodeMargin;
	FString* IconName;
	FLinearColor* NodeColor;
};