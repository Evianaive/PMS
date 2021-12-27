// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPanel.h"
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"

/**
 * 
 */


class SPMSGraphPanel : public SGraphPanel
{
public:
	
	// SLATE_BEGIN_ARGS(SPMSGraphPanel) {}
	//
	// SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave( const FDragDropEvent& DragDropEvent ) override;
	virtual FReply OnDragOver( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual FReply OnDrop( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FReply OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent ) override;

	virtual bool OnHandleLeftMouseRelease(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	/** Handle updating the spline hover state */
	void OnSplineHoverStateChanged(const FGraphSplineOverlapResult& NewSplineHoverState);
	void PaintBackgroundAsLines(const FSlateBrush* BackgroundImage, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32& DrawLayerId) const;
	SGraphPin* GetBestPinFromHoveredSpline() const;
	bool PassesAssetReferenceFilter(const TArray<FAssetData>& ReferencedAssets, FText* OutFailureReason = nullptr) const;

	EContextEnterState ContextEnterState = EContextEnterState::None;
	EMouseEnterState MouseEnterState = EMouseEnterState::None;

	TArray<UPMSEdGraphNode*> MoveTogetherNodes;
	TArray<FVector2D> MoveTogetherNodesStartPos;
	
	FVector2D MouseMovementAfterDown = FVector2D::ZeroVector;
};