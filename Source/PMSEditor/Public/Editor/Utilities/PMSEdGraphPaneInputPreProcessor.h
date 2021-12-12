// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/PMSEdGraphNode.h"
#include "Framework/Application/IInputProcessor.h"
#include "Layout/Geometry.h"

class SGraphPin;
class SPMSEdGraphNode;
class SPMSEdGraphPin;
class SGraphPanel;

struct FPMSEventContex
{
	bool IsClickGesture = false;
	bool IsDoubleClickGesture = false;

	bool IsCursorInsidePanel = false;
	bool IsCursorOnPanelEmptySpace = false;

	bool IsDraggingConnection = false;
	bool IsDragDropping = false;
	bool IsDraggingNode = false;
	bool IsPanning = false;
	bool IsBoxSelecting = false;
	
	TSharedPtr<SGraphPanel> GraphPanel = nullptr; //hovered graph panel.
	FGeometry PanelGeometry;
	TSharedPtr<SPMSEdGraphNode> GraphNode = nullptr; //hovered graph node.
	TSharedPtr<SPMSEdGraphNode> CommentNode = nullptr; 
	FGeometry NodeGeometry;
	bool IsNodeTitle = false;
	TSharedPtr<SPMSEdGraphPin> GraphPin = nullptr; //hovered graph pin.
	FGeometry PinGeometry;
	bool IsInPinEditableBox = false; //[editable text pin]
};

struct FPMSEventReply
{
	bool IsHandled = false;
	bool ShouldBlockInput = false;

	static FPMSEventReply BlockSlateInput()
	{
		FPMSEventReply reply;
		reply.IsHandled = true;
		reply.ShouldBlockInput = true;
		return reply;
	};

	static FPMSEventReply UnHandled()
	{
		return FPMSEventReply();
	}
	static FPMSEventReply Handled()
	{
		FPMSEventReply reply;
		reply.IsHandled = true;
		return reply;
	}

	void Append(FPMSEventReply AdditionalReply)
	{
		IsHandled |= AdditionalReply.IsHandled;
		ShouldBlockInput |= AdditionalReply.ShouldBlockInput;
	}
};

class FPMSEdGraphPaneInputPreProcessor : public IInputProcessor
{
public:
	FPMSEdGraphPaneInputPreProcessor();
	virtual ~FPMSEdGraphPaneInputPreProcessor();

	
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
	
	FPMSEventContex InitEventContext(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);
	TSharedPtr<SGraphPanel> GetCurrentGraphPanel();
	// CurrentGraphPanel;
	FVector2D GraphPosToScreenPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D PanelPos);
	FVector2D ScreenPosToGraphPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D ScreenPos);

	void OnSelectLinkedNodes(bool bDownStream, bool bUpStream);
private:
	struct ShakeOffNodeTrackigInfo
	{
		double MouseMoveTime;
		FVector2D MouseMoveDirection;
	};
	TArray<ShakeOffNodeTrackigInfo> ShakeOffNodeTracker;

	FVector2D LastGraphCursorGraphPos;
	FVector2D LastGraphCursorScreenPosClamped;
	FSlateRect LastPanelScreenSpaceRect;

	FVector2D LastMouseDownScreenPos = FVector2D::ZeroVector;
	FVector2D LastMouseUpScreenPos = FVector2D::ZeroVector;
	float MouseUpDeltaTime = 0.f;

	bool DidIHideTheCursor = false;
	bool HasBegunCuttingWire = false;
	bool HasBegunLazyConnect = false;
	bool RefreshToolTipWhenMouseMove = false;
	bool BlockNextClick = false;

	TWeakPtr<SPMSEdGraphNode> NodeBeingDrag;
	TWeakPtr<SPMSEdGraphNode> CommentNodeBeingDrag;
	TArray<UPMSEdGraphNode*> SameActionNodes;

	DECLARE_DELEGATE_RetVal(bool, PMSDeferredEventDele)
	TArray<PMSDeferredEventDele> TickEventListener;


	TSet<TWeakPtr<SPMSEdGraphPin>> HighLightedPins;
	TSet<TWeakPtr<SPMSEdGraphNode>> HighLightedNodes;

	//use when need to boost slate performance.
	FThrottleRequest MouseButtonDownResponsivnessThrottle;

	//for setting input schem in editor ui;
	TSharedPtr<FUICommandList> UICommandList;

	void* CusorResource_Scissor;
	int32 PressedCharKey = 0;

	TSet<FKey> DragNPanPressedKeys;
};

