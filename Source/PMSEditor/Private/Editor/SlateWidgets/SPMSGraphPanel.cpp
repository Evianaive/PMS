#include "Editor/SlateWidgets/SPMSGraphPanel.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphPanelPublic.h"

#include "Rendering/DrawElements.h"
#include "EdGraph/EdGraph.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "EdGraphNode_Comment.h"
#include "Settings/EditorExperimentalSettings.h"
#include "Editor.h"
#include "GraphEditorSettings.h"
#include "GraphEditorDragDropAction.h"
#include "NodeFactory.h"
#include "Classes/EditorStyleSettings.h"

#include "DragAndDrop/DecoratedDragDropOp.h"
#include "DragAndDrop/ActorDragDropGraphEdOp.h"
#include "DragAndDrop/AssetDragDropOp.h"
#include "DragAndDrop/LevelDragDropOp.h"
#include "DragAndDrop/GraphNodeDragDropOp.h"

#include "GraphEditorActions.h"

#include "ConnectionDrawingPolicy.h"

#include "AssetSelection.h"

#include "KismetNodes/KismetNodeInfoContext.h"
#include "GraphDiffControl.h"

#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "ScopedTransaction.h"
#include "Editor/PMSEdGraphNode.h"
#include "Editor/PMSEditorSettings.h"

namespace NodePanelDefs
{
	// Default Zoom Padding Value
	static const float DefaultZoomPadding = 25.f;
	// Node Culling Guardband Area
	static const float GuardBandArea = 0.25f;
	// Scaling factor to reduce speed of mouse zooming
	static const float MouseZoomScaling = 0.04f;
};

void SPMSGraphPanel::Construct(const FArguments& InArgs)
{
	SGraphPanel::Construct(InArgs);
}

FReply SPMSGraphPanel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UE_LOG(LogTemp,Log,TEXT("Effecting is %s"),ToCStr(MouseEvent.GetEffectingButton().ToString()));

	
	/*SNodePanel*/
	const bool bIsLeftMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
	const bool bIsRightMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
	const bool bIsMiddleMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton;

	/*Mouse Enter State*/
	{
		if (bIsLeftMouseButtonEffecting)
		{
			MouseEnterState = EMouseEnterState::Left;		
		}
		else if(bIsMiddleMouseButtonEffecting)
		{
			MouseEnterState = EMouseEnterState::Middle;
		}
		else if(bIsRightMouseButtonEffecting)
		{
			MouseEnterState = EMouseEnterState::Right;
		}
		else
		{
			MouseEnterState = EMouseEnterState::None;
		}
	}
	TotalMouseDelta = 0;
	
	/*OnConnection Click*/
	/*SGraphPanel*/
	if ((MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton))
	{
		if (SGraphPin* BestPinFromHoveredSpline = GetBestPinFromHoveredSpline())
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				ContextEnterState = EContextEnterState::OnLine;
				return BestPinFromHoveredSpline->OnPinMouseDown(MyGeometry, MouseEvent);				
			}
		}
	}
	
	
	if(bIsRightMouseButtonEffecting)
	{
		// Starting zoom by holding RMB
		FReply ReplyState = FReply::Handled();
		ReplyState.CaptureMouse( SharedThis(this) );
		ReplyState.UseHighPrecisionMouseMovement( SharedThis(this) );
	
		DeferredMovementTargetObject = nullptr; // clear any interpolation when you manually zoom
		CancelZoomToFit();
		TotalMouseDeltaXY = 0;
	
		if (!FSlateApplication::Get().IsUsingTrackpad()) // on trackpad we don't know yet if user wants to zoom or bring up the context menu
		{
			bShowSoftwareCursor = true;
		}
	
		if (bIsLeftMouseButtonEffecting)
		{
			// Got here from panning mode (with RMB held) - clear panning mode, but use cached software cursor position
			const FVector2D WidgetSpaceCursorPos = GraphCoordToPanelCoord( SoftwareCursorPosition );
			ZoomStartOffset = WidgetSpaceCursorPos;
			this->bIsPanning = false;
		}
		else
		{
			// Cache current cursor position as zoom origin and software cursor position
			ZoomStartOffset = MyGeometry.AbsoluteToLocal( MouseEvent.GetLastScreenSpacePosition() );
			SoftwareCursorPosition = PanelCoordToGraphCoord( ZoomStartOffset );
	
			if (bIsRightMouseButtonEffecting)
			{
				// Clear things that may be set when left clicking
				if (NodeUnderMousePtr.IsValid())
				{
					OnEndNodeInteraction(NodeUnderMousePtr.Pin().ToSharedRef());
				}
	
				if ( Marquee.IsValid() )
				{
					auto PreviouslySelectedNodes = SelectionManager.SelectedNodes;
					ApplyMarqueeSelection(Marquee, PreviouslySelectedNodes, SelectionManager.SelectedNodes);
					if (SelectionManager.SelectedNodes.Num() > 0 || PreviouslySelectedNodes.Num() > 0)
					{
						SelectionManager.OnSelectionChanged.ExecuteIfBound(SelectionManager.SelectedNodes);
					}
				}
	
				Marquee = FMarqueeOperation();
			}
		}
	
		return ReplyState;
	}
	else if (bIsMiddleMouseButtonEffecting)
	{
		// Cache current cursor position as zoom origin and software cursor position
		ZoomStartOffset = MyGeometry.AbsoluteToLocal(MouseEvent.GetLastScreenSpacePosition());
		SoftwareCursorPosition = PanelCoordToGraphCoord(ZoomStartOffset);
	
		FReply ReplyState = FReply::Handled();
		ReplyState.CaptureMouse(SharedThis(this));
		ReplyState.UseHighPrecisionMouseMovement(SharedThis(this));
	
		SoftwareCursorPosition = PanelCoordToGraphCoord(MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()));
	
		DeferredMovementTargetObject = nullptr; // clear any interpolation when you manually pan
		CancelZoomToFit();
	
		// MIDDLE BUTTON is for dragging only.
		return ReplyState;
	}
	else if ( bIsLeftMouseButtonEffecting )
	{
		// LEFT BUTTON is for selecting nodes and manipulating pins.
		FArrangedChildren ArrangedChildren(EVisibility::Visible);
		ArrangeChildNodes(MyGeometry, ArrangedChildren);
	
		const int32 NodeUnderMouseIndex = SWidget::FindChildUnderMouse( ArrangedChildren, MouseEvent );
		if ( NodeUnderMouseIndex != INDEX_NONE )
		{
			// PRESSING ON A NODE!
	
			// This changes selection and starts dragging it.
			const FArrangedWidget& NodeGeometry = ArrangedChildren[NodeUnderMouseIndex];
			const FVector2D MousePositionInNode = NodeGeometry.Geometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
			TSharedRef<SNode> NodeWidgetUnderMouse = StaticCastSharedRef<SNode>( NodeGeometry.Widget );
	
			if( NodeWidgetUnderMouse->CanBeSelected(MousePositionInNode) )
			{
				// Track the node that we're dragging; we will move it in OnMouseMove.
				//this->OnBeginNodeInteraction(NodeWidgetUnderMouse, MousePositionInNode);
				NodeUnderMousePtr = NodeWidgetUnderMouse;
				NodeGrabOffset = MousePositionInNode;
				ContextEnterState = EContextEnterState::OnNode;
				return FReply::Handled().CaptureMouse( SharedThis(this) );
			}
		}
	
		// START MARQUEE SELECTION.
		const FVector2D GraphMousePos = PanelCoordToGraphCoord( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) );
		Marquee.Start( GraphMousePos, FMarqueeOperation::OperationTypeFromMouseEvent(MouseEvent) );
	
		// If we're marquee selecting, then we're not clicking on a node!
		//NodeUnderMousePtr.Reset();
		ContextEnterState = EContextEnterState::OnSpace;
	
		return FReply::Handled().CaptureMouse( SharedThis(this) );
	}
	else
	{
		return FReply::Unhandled();
	}
	//return SGraphPanel::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SPMSGraphPanel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!NodeUnderMousePtr.IsValid() && !Marquee.IsValid() && (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) && (MouseEvent.IsShiftDown()))
	{
		if (SGraphPin* BestPinFromHoveredSpline = GetBestPinFromHoveredSpline())
		{
			return BestPinFromHoveredSpline->OnMouseButtonUp(MyGeometry, MouseEvent);
		}
	}


	FReply ReplyState = FReply::Unhandled();

	const bool bIsLeftMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
	const bool bIsRightMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
	const bool bIsMiddleMouseButtonEffecting = MouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton;
	const bool bIsRightMouseButtonDown = MouseEvent.IsMouseButtonDown( EKeys::RightMouseButton );
	const bool bIsLeftMouseButtonDown = MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton );
	const bool bIsMiddleMouseButtonDown = MouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton);

	// Did the user move the cursor sufficiently far, or is it in a dead zone?
	// In Dead zone     - implies actions like summoning context menus and general clicking.
	// Out of Dead Zone - implies dragging actions like moving nodes and marquee selection.
	const bool bCursorInDeadZone = TotalMouseDelta <= FSlateApplication::Get().GetDragTriggerDistance();

	// Set to true later if we need to finish with the software cursor
	bool bRemoveSoftwareCursor = false;

	if ((bIsLeftMouseButtonEffecting && bIsRightMouseButtonDown)
	||  (bIsRightMouseButtonEffecting && (bIsLeftMouseButtonDown || (FSlateApplication::Get().IsUsingTrackpad() && bIsZoomingWithTrackpad)))
	||	(bIsMiddleMouseButtonEffecting && bIsRightMouseButtonDown))
	{
		// Ending zoom by releasing LMB or RMB
		ReplyState = FReply::Handled();

		if (bIsLeftMouseButtonDown || FSlateApplication::Get().IsUsingTrackpad())
		{
			// If we released the right mouse button first, we need to cancel the software cursor display
			bRemoveSoftwareCursor = true;
			bIsZoomingWithTrackpad = false;
			ReplyState.ReleaseMouseCapture();
		}
	}
	else if ( bIsRightMouseButtonEffecting )
	{
		ReplyState = FReply::Handled().ReleaseMouseCapture();

		bRemoveSoftwareCursor = true;

		TSharedPtr<SWidget> WidgetToFocus;
		if (bCursorInDeadZone)
		{
			WidgetToFocus = OnSummonContextMenu(MyGeometry, MouseEvent);
		}

		this->bIsPanning = false;

		if (WidgetToFocus.IsValid())
		{
			ReplyState.SetUserFocus(WidgetToFocus.ToSharedRef(), EFocusCause::SetDirectly);
		}
	}
	else if ( bIsMiddleMouseButtonEffecting )
	{
		ReplyState = FReply::Handled().ReleaseMouseCapture();
		
		bRemoveSoftwareCursor = true;
		
		this->bIsPanning = false;
	}
	else if ( bIsLeftMouseButtonEffecting )
	{
		if (NodeUnderMousePtr.IsValid())
		{
			OnEndNodeInteraction(NodeUnderMousePtr.Pin().ToSharedRef());

			FinalizeNodeMovements();
			ScopedTransactionPtr.Reset();
		}
				
		if (OnHandleLeftMouseRelease(MyGeometry, MouseEvent))
		{

		}
		else if ( bCursorInDeadZone )
		{
			//@TODO: Move to selection manager
			if ( NodeUnderMousePtr.IsValid() )
			{
				// We clicked on a node!
				TSharedRef<SNode> NodeWidgetUnderMouse = NodeUnderMousePtr.Pin().ToSharedRef();

				SelectionManager.ClickedOnNode(NodeWidgetUnderMouse->GetObjectBeingDisplayed(), MouseEvent);

				// We're done interacting with this node.
				NodeUnderMousePtr.Reset();
			}
			else if (this->HasMouseCapture())
			{
				// We clicked on the panel background
				this->SelectionManager.ClearSelectionSet();

				if(OnSpawnNodeByShortcut.IsBound())
				{
					OnSpawnNodeByShortcut.Execute(LastKeyChordDetected, PanelCoordToGraphCoord(  MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) ));
				}

				LastKeyChordDetected = FInputChord();
			}
		}
		else if ( Marquee.IsValid() )
		{
			auto PreviouslySelectedNodes = SelectionManager.SelectedNodes;
			ApplyMarqueeSelection(Marquee, PreviouslySelectedNodes, SelectionManager.SelectedNodes);
			if (SelectionManager.SelectedNodes.Num() > 0 || PreviouslySelectedNodes.Num() > 0)
			{
				SelectionManager.OnSelectionChanged.ExecuteIfBound(SelectionManager.SelectedNodes);
			}
		}

		// The existing marquee operation ended; reset it.
		Marquee = FMarqueeOperation();

		ReplyState = FReply::Handled().ReleaseMouseCapture();
	}

	if (bRemoveSoftwareCursor)
	{
		// If we released the right mouse button first, we need to cancel the software cursor display
		if ( this->HasMouseCapture() )
		{
			FSlateRect ThisPanelScreenSpaceRect = MyGeometry.GetLayoutBoundingRect();
			const FVector2D ScreenSpaceCursorPos = MyGeometry.LocalToAbsolute( GraphCoordToPanelCoord( SoftwareCursorPosition ) );

			FIntPoint BestPositionInViewport(
				FMath::RoundToInt( FMath::Clamp( ScreenSpaceCursorPos.X, ThisPanelScreenSpaceRect.Left, ThisPanelScreenSpaceRect.Right ) ),
				FMath::RoundToInt( FMath::Clamp( ScreenSpaceCursorPos.Y, ThisPanelScreenSpaceRect.Top, ThisPanelScreenSpaceRect.Bottom ) )
				);

			if (!bCursorInDeadZone)
			{
				ReplyState.SetMousePos(BestPositionInViewport);
			}
		}

		bShowSoftwareCursor = false;
	}

	return ReplyState;	
	//return SGraphPanel::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SPMSGraphPanel::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp,Log,TEXT("SPMSGraphPanel"));

	UEdGraphPin* Pin1;
	UEdGraphPin* Pin2;
	if (PreviousFrameSplineOverlap.GetPins(*this, /*out*/ Pin1, /*out*/ Pin2))
	{
		// Give the schema a chance to do something interesting with a double click on a proper spline (both ends are attached to a pin, i.e., not a preview/drag one)
		const FVector2D DoubleClickPositionInGraphSpace = PanelCoordToGraphCoord(InMyGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition()));

		const UEdGraphSchema* Schema = GraphObj->GetSchema();
		Schema->OnPinConnectionDoubleCicked(Pin1, Pin2, DoubleClickPositionInGraphSpace);
	}
	else if (!PreviousFrameSplineOverlap.GetCloseToSpline())
	{
		OnDoubleClicked.ExecuteIfBound();
	}

	return SNodePanel::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SPMSGraphPanel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	LastPointerEvent = MouseEvent;
	LastPointerGeometry = MyGeometry;

	// Save the mouse position to use in OnPaint for spline hit detection
	SavedMousePosForOnPaintEventLocalSpace = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());

	// Invalidate the spline results if we moved very far
	{
		const FVector2D MouseDelta = SavedMousePosForOnPaintEventLocalSpace - PreviousFrameSavedMousePosForSplineOverlap;
		const float MouseDeltaLengthSquared = MouseDelta.SizeSquared();
		const bool bCursorInDeadZone = MouseDeltaLengthSquared <= FMath::Square(FSlateApplication::Get().GetDragTriggerDistance());

		if (!bCursorInDeadZone)
		{
			//@TODO: Should we do this or just rely on the next OnPaint?
			// Our frame-latent approximation is going to be totally junk if the mouse is moving quickly
			OnSplineHoverStateChanged(FGraphSplineOverlapResult());
		}		
	}


	
	const bool bIsRightMouseButtonDown = MouseEvent.IsMouseButtonDown( EKeys::RightMouseButton );
	const bool bIsLeftMouseButtonDown = MouseEvent.IsMouseButtonDown( EKeys::LeftMouseButton );
	const bool bIsMiddleMouseButtonDown = MouseEvent.IsMouseButtonDown(EKeys::MiddleMouseButton);
	const FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();

	PastePosition = PanelCoordToGraphCoord( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) );

	if ( this->HasMouseCapture() )
	{
		const FVector2D CursorDelta = MouseEvent.GetCursorDelta();
		// Track how much the mouse moved since the mouse down.
		TotalMouseDelta += CursorDelta.Size();

		const bool bShouldZoom = bIsRightMouseButtonDown && (bIsLeftMouseButtonDown || bIsMiddleMouseButtonDown || ModifierKeysState.IsAltDown() || FSlateApplication::Get().IsUsingTrackpad());
		if (bShouldZoom)
		{
			FReply ReplyState = FReply::Handled();

			TotalMouseDeltaXY += CursorDelta.X + CursorDelta.Y;

			const int32 ZoomLevelDelta = FMath::RoundToInt(TotalMouseDeltaXY * NodePanelDefs::MouseZoomScaling);

			// Get rid of mouse movement that's been 'used up' by zooming
			if (ZoomLevelDelta != 0)
			{
				TotalMouseDeltaXY -= (ZoomLevelDelta / NodePanelDefs::MouseZoomScaling);
			}

			// Perform zoom centered on the cached start offset
			ChangeZoomLevel(ZoomLevelDelta, ZoomStartOffset, MouseEvent.IsControlDown());

			this->bIsPanning = false;

			if (FSlateApplication::Get().IsUsingTrackpad() && ZoomLevelDelta != 0)
			{
				this->bIsZoomingWithTrackpad = true;
				bShowSoftwareCursor = true;
			}

			// Stop the zoom-to-fit in favor of user control
			CancelZoomToFit();

			return ReplyState;
		}
		else if (bIsRightMouseButtonDown)
		{
			FReply ReplyState = FReply::Handled();

			if( !CursorDelta.IsZero() )
			{
				bShowSoftwareCursor = true;
			}

			// Panning and mouse is outside of panel? Pasting should just go to the screen center.
			PastePosition = PanelCoordToGraphCoord( 0.5f * MyGeometry.GetLocalSize() );

			this->bIsPanning = true;
			ViewOffset -= CursorDelta / GetZoomAmount();

			// Stop the zoom-to-fit in favor of user control
			CancelZoomToFit();

			return ReplyState;
		}
		else if (bIsMiddleMouseButtonDown)
		{
			FReply ReplyState = FReply::Handled();

			if (!CursorDelta.IsZero())
			{
				bShowSoftwareCursor = true;
			}

			// Panning and mouse is outside of panel? Pasting should just go to the screen center.
			PastePosition = PanelCoordToGraphCoord(0.5f * MyGeometry.Size);

			this->bIsPanning = true;
			ViewOffset -= CursorDelta / GetZoomAmount();

			return ReplyState;
		}
		else if (bIsLeftMouseButtonDown)
		{
			TSharedPtr<SNode> NodeBeingDragged = NodeUnderMousePtr.Pin();

			if ( IsEditable.Get() )
			{
				// Update the amount to pan panel
				UpdateViewOffset(MyGeometry, MouseEvent.GetScreenSpacePosition());

				const bool bCursorInDeadZone = TotalMouseDelta <= FSlateApplication::Get().GetDragTriggerDistance();

				if ( NodeBeingDragged.IsValid() )
				{
					if ( !bCursorInDeadZone )
					{
						// Note, NodeGrabOffset() comes from the node itself, so it's already scaled correctly.
						FVector2D AnchorNodeNewPos = PanelCoordToGraphCoord( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) ) - NodeGrabOffset;

						// Snap to grid
						const float SnapSize = GetDefault<UPMSEditorSettings>()->MoveStepSize;
						AnchorNodeNewPos.X = SnapSize * FMath::RoundToFloat( AnchorNodeNewPos.X / SnapSize );
						AnchorNodeNewPos.Y = SnapSize * FMath::RoundToFloat( AnchorNodeNewPos.Y / SnapSize );

						// Dragging an unselected node automatically selects it.
						//SelectionManager.StartDraggingNode(NodeBeingDragged->GetObjectBeingDisplayed(), MouseEvent);
						
						// Move all the selected nodes.
						{
							const FVector2D AnchorNodeOldPos = NodeBeingDragged->GetPosition();
							const FVector2D DeltaPos = AnchorNodeNewPos - AnchorNodeOldPos;

							// Perform movement in 3 passes:
							
							// 1. Gather all selected nodes positions and calculate new positions
							struct FDefferedNodePosition 
							{ 
								SNode*		Node; 
								FVector2D	NewPosition; 
							};
							TArray<FDefferedNodePosition> DefferedNodesToMove;

							// 2. Deffer actual move transactions to mouse release or focus lost
							bool bStoreOriginalNodePositions = OriginalNodePositions.Num() == 0;
							for (FGraphPanelSelectionSet::TIterator NodeIt(SelectionManager.SelectedNodes); NodeIt; ++NodeIt)
							{
								if (TSharedRef<SNode>* pWidget = NodeToWidgetLookup.Find(*NodeIt))
								{
									SNode& Widget = pWidget->Get();
									FDefferedNodePosition NodePosition = { &Widget, Widget.GetPosition() + DeltaPos };
									DefferedNodesToMove.Add(NodePosition);

									if (bStoreOriginalNodePositions)
									{
										OriginalNodePositions.FindOrAdd(*pWidget) = Widget.GetPosition();
									}
								}
							}

							// 3. Move selected nodes to new positions
							SNode::FNodeSet NodeFilter;

							for (int32 NodeIdx = 0; NodeIdx < DefferedNodesToMove.Num(); ++NodeIdx)
							{
								DefferedNodesToMove[NodeIdx].Node->MoveTo(DefferedNodesToMove[NodeIdx].NewPosition, NodeFilter, false);
							}
						}
					}

					return FReply::Handled();
				}
			}

			if ( !NodeBeingDragged.IsValid() )
			{
				// We are marquee selecting
				const FVector2D GraphMousePos = PanelCoordToGraphCoord( MyGeometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ) );
				Marquee.Rect.UpdateEndPoint(GraphMousePos);

				FindNodesAffectedByMarquee( /*out*/ Marquee.AffectedNodes );
				return FReply::Handled();
			}

			// Stop the zoom-to-fit in favor of user control
			CancelZoomToFit();
		}
	}

	return FReply::Unhandled();
}

void SPMSGraphPanel::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FGraphEditorDragDropAction> DragConnectionOp = DragDropEvent.GetOperationAs<FGraphEditorDragDropAction>();
	if (DragConnectionOp.IsValid())
	{
		DragConnectionOp->SetHoveredGraph( SharedThis(this) );
	}
}

void SPMSGraphPanel::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FGraphEditorDragDropAction> Operation = DragDropEvent.GetOperationAs<FGraphEditorDragDropAction>();
	if( Operation.IsValid() )
	{
		Operation->SetHoveredGraph(TSharedPtr<SGraphPanel>(nullptr));
	}
	else
	{
		TSharedPtr<FDecoratedDragDropOp> AssetOp = DragDropEvent.GetOperationAs<FDecoratedDragDropOp>();
		if( AssetOp.IsValid()  )
		{
			AssetOp->ResetToDefaultToolTip();
		}
	}
}

FReply SPMSGraphPanel::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return FReply::Unhandled();
	}

	// Handle Read only graphs
	if (!IsEditable.Get())
	{
		TSharedPtr<FGraphEditorDragDropAction> GraphDragDropOp = DragDropEvent.GetOperationAs<FGraphEditorDragDropAction>();

		if (GraphDragDropOp.IsValid())
		{
			GraphDragDropOp->SetDropTargetValid(false);
		}
		else
		{
			TSharedPtr<FDecoratedDragDropOp> AssetOp = DragDropEvent.GetOperationAs<FDecoratedDragDropOp>();
			if (AssetOp.IsValid())
			{
				FText Tooltip = AssetOp->GetHoverText();
				if (Tooltip.IsEmpty())
				{
					Tooltip = NSLOCTEXT( "GraphPanel", "DragDropOperation", "Graph is Read-Only" );
				}
				AssetOp->SetToolTip(Tooltip, FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error")));
			}
		}
		return FReply::Handled();
	}

	if (Operation->IsOfType<FGraphEditorDragDropAction>())
	{
		PreviewConnectorEndpoint = MyGeometry.AbsoluteToLocal( DragDropEvent.GetScreenSpacePosition() );
		return FReply::Handled();
	}
	else if (Operation->IsOfType<FExternalDragOperation>())
	{
		return AssetUtil::CanHandleAssetDrag(DragDropEvent);
	}
	else if (Operation->IsOfType<FAssetDragDropOp>())
	{
		if ((GraphObj != nullptr) && (GraphObj->GetSchema() != nullptr))
		{
			TSharedPtr<FAssetDragDropOp> AssetOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
			bool bOkIcon = false;
			FText TooltipText;
			if (AssetOp->HasAssets())
			{
				const TArray<FAssetData>& HoveredAssetData = AssetOp->GetAssets();
				FText AssetReferenceFilterFailureReason;
				if (PassesAssetReferenceFilter(HoveredAssetData, &AssetReferenceFilterFailureReason))
				{
					FString TooltipTextString;
					GraphObj->GetSchema()->GetAssetsGraphHoverMessage(HoveredAssetData, GraphObj, /*out*/ TooltipTextString, /*out*/ bOkIcon);
					TooltipText = FText::FromString(TooltipTextString);
				}
				else
				{
					TooltipText = AssetReferenceFilterFailureReason;
					bOkIcon = false;
				}
			}
			const FSlateBrush* TooltipIcon = bOkIcon ? FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK")) : FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error"));
			AssetOp->SetToolTip(TooltipText, TooltipIcon);
		}
		return FReply::Handled();
	} 
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPMSGraphPanel::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	const FVector2D NodeAddPosition = PanelCoordToGraphCoord( MyGeometry.AbsoluteToLocal( DragDropEvent.GetScreenSpacePosition() ) );

	FSlateApplication::Get().SetKeyboardFocus(AsShared(), EFocusCause::SetDirectly);

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid() || !IsEditable.Get())
	{
		return FReply::Unhandled();
	}

	if (Operation->IsOfType<FGraphEditorDragDropAction>())
	{
		check(GraphObj);
		TSharedPtr<FGraphEditorDragDropAction> DragConn = StaticCastSharedPtr<FGraphEditorDragDropAction>(Operation);
		if (DragConn.IsValid() && DragConn->IsSupportedBySchema(GraphObj->GetSchema()))
		{
			return DragConn->DroppedOnPanel(SharedThis(this), DragDropEvent.GetScreenSpacePosition(), NodeAddPosition, *GraphObj);
		}

		return FReply::Unhandled();
	}
	else if (Operation->IsOfType<FActorDragDropGraphEdOp>())
	{
		TSharedPtr<FActorDragDropGraphEdOp> ActorOp = StaticCastSharedPtr<FActorDragDropGraphEdOp>(Operation);
		OnDropActor.ExecuteIfBound(ActorOp->Actors, GraphObj, NodeAddPosition);
		return FReply::Handled();
	}

	else if (Operation->IsOfType<FLevelDragDropOp>())
	{
		TSharedPtr<FLevelDragDropOp> LevelOp = StaticCastSharedPtr<FLevelDragDropOp>(Operation);
		OnDropStreamingLevel.ExecuteIfBound(LevelOp->StreamingLevelsToDrop, GraphObj, NodeAddPosition);
		return FReply::Handled();
	}

	else if (Operation->IsOfType<FGraphNodeDragDropOp>())
	{
		TSharedPtr<FGraphNodeDragDropOp> NodeDropOp = StaticCastSharedPtr<FGraphNodeDragDropOp>(Operation);
		NodeDropOp->OnPerformDropToGraph.ExecuteIfBound(NodeDropOp, GraphObj, NodeAddPosition, DragDropEvent.GetScreenSpacePosition());
		return FReply::Handled();
	}
	else
	{
		if ((GraphObj != nullptr) && (GraphObj->GetSchema() != nullptr))
		{
			TArray< FAssetData > DroppedAssetData = AssetUtil::ExtractAssetDataFromDrag( DragDropEvent );

			if ( DroppedAssetData.Num() > 0 )
			{
				if (PassesAssetReferenceFilter(DroppedAssetData))
				{
					GraphObj->GetSchema()->DroppedAssetsOnGraph( DroppedAssetData, NodeAddPosition, GraphObj );
				}
				return FReply::Handled();
			}
		}

		return FReply::Unhandled();
	}
}

int32 SPMSGraphPanel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
                              const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                              const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	CachedAllottedGeometryScaledSize = AllottedGeometry.GetLocalSize() * AllottedGeometry.Scale;

	//Style used for objects that are the same between revisions
	FWidgetStyle FadedStyle = InWidgetStyle;
	FadedStyle.BlendColorAndOpacityTint(FLinearColor(0.45f,0.45f,0.45f,0.45f));

	// First paint the background
	const UEditorExperimentalSettings& Options = *GetDefault<UEditorExperimentalSettings>();

	const FSlateBrush* DefaultBackground = FEditorStyle::GetBrush(TEXT("Graph.Panel.SolidBackground"));
	const FSlateBrush* CustomBackground = &GetDefault<UEditorStyleSettings>()->GraphBackgroundBrush;
	const FSlateBrush* BackgroundImage = CustomBackground->HasUObject() ? CustomBackground : DefaultBackground;
	PaintBackgroundAsLines(BackgroundImage, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId);

	const float ZoomFactor = AllottedGeometry.Scale * GetZoomAmount();

	FArrangedChildren ArrangedChildren(EVisibility::Visible);
	ArrangeChildNodes(AllottedGeometry, ArrangedChildren);

	// Determine some 'global' settings based on current LOD
	const bool bDrawShadowsThisFrame = GetCurrentLOD() > EGraphRenderingLOD::LowestDetail;

	// Because we paint multiple children, we must track the maximum layer id that they produced in case one of our parents
	// wants to an overlay for all of its contents.

	// Save LayerId for comment boxes to ensure they always appear below nodes & wires
	const int32 CommentNodeShadowLayerId = LayerId++;
	const int32 CommentNodeLayerId = LayerId++;

	// Save a LayerId for wires, which appear below nodes but above comments
	// We will draw them later, along with the arrows which appear above nodes.
	const int32 WireLayerId = LayerId++;

	const int32 NodeShadowsLayerId = LayerId;
	const int32 NodeLayerId = NodeShadowsLayerId + 1;
	int32 MaxLayerId = NodeLayerId;

	const FPaintArgs NewArgs = Args.WithNewParent(this);

	const FVector2D NodeShadowSize = GetDefault<UGraphEditorSettings>()->GetShadowDeltaSize();
	const UEdGraphSchema* Schema = GraphObj->GetSchema();

	// Draw the child nodes
	{
		// When drawing a marquee, need a preview of what the selection will be.
		const FGraphPanelSelectionSet* SelectionToVisualize = &(SelectionManager.SelectedNodes);
		FGraphPanelSelectionSet SelectionPreview;
		if ( Marquee.IsValid() )
		{			
			ApplyMarqueeSelection(Marquee, SelectionManager.SelectedNodes, SelectionPreview);
			SelectionToVisualize = &SelectionPreview;
		}

		// Context for rendering node infos
		// FKismetNodeInfoContext Context(GraphObj);

		TArray<FGraphDiffControl::FNodeMatch> NodeMatches;
		for (int32 ChildIndex = 0; ChildIndex < ArrangedChildren.Num(); ++ChildIndex)
		{
			FArrangedWidget& CurWidget = ArrangedChildren[ChildIndex];
			TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
			
			// Examine node to see what layers we should be drawing in
			int32 ShadowLayerId = NodeShadowsLayerId;
			int32 ChildLayerId = NodeLayerId;

			// If a comment node, draw in the dedicated comment slots
			{
				UObject* NodeObj = ChildNode->GetObjectBeingDisplayed();
				if (NodeObj && NodeObj->IsA(UEdGraphNode_Comment::StaticClass()))
				{
					ShadowLayerId = CommentNodeShadowLayerId;
					ChildLayerId = CommentNodeLayerId;
				}
			}


			const bool bNodeIsVisible = FSlateRect::DoRectanglesIntersect( CurWidget.Geometry.GetLayoutBoundingRect(), MyCullingRect );

			if (bNodeIsVisible)
			{
				const bool bSelected = SelectionToVisualize->Contains( StaticCastSharedRef<SNodePanel::SNode>(CurWidget.Widget)->GetObjectBeingDisplayed() );
				
				UEdGraphNode* NodeObj = Cast<UEdGraphNode>(ChildNode->GetObjectBeingDisplayed());
				float Alpha = 1.0f;

				// Handle Node renaming once the node is visible
				if( bSelected && ChildNode->IsRenamePending() )
				{
					// Only open a rename when the window has focus
					TSharedPtr<SWindow> OwnerWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
					if (!OwnerWindow.IsValid() || FSlateApplication::Get().HasFocusedDescendants(OwnerWindow.ToSharedRef()))
					{
						ChildNode->ApplyRename();
					}
				}

				// Draw the node's shadow.
				if (bDrawShadowsThisFrame || bSelected)
				{
					const FSlateBrush* ShadowBrush = ChildNode->GetShadowBrush(bSelected);
					FSlateDrawElement::MakeBox(
						OutDrawElements,
						ShadowLayerId,
						CurWidget.Geometry.ToInflatedPaintGeometry(NodeShadowSize),
						ShadowBrush,
						ESlateDrawEffect::None,
						FLinearColor(1.0f, 1.0f, 1.0f, Alpha)
						);
				}

				// // Draw the comments and information popups for this node, if it has any.
				// {
				// 	const SNodePanel::SNode::FNodeSlot* CommentSlot = ChildNode->GetSlot( ENodeZone::TopCenter );
				// 	float CommentBubbleY = CommentSlot ? -CommentSlot->GetSlotOffset().Y : 0.f;
				// 	Context.bSelected = bSelected;
				// 	TArray<FGraphInformationPopupInfo> Popups;
				//
				// 	{
				// 		ChildNode->GetNodeInfoPopups(&Context, /*out*/ Popups);
				// 	}
				//
				// 	for (int32 PopupIndex = 0; PopupIndex < Popups.Num(); ++PopupIndex)
				// 	{
				// 		FGraphInformationPopupInfo& Popup = Popups[PopupIndex];
				// 		PaintComment(Popup.Message, CurWidget.Geometry, MyCullingRect, OutDrawElements, ChildLayerId, Popup.BackgroundColor, /*inout*/ CommentBubbleY, InWidgetStyle);
				// 	}
				// }
				
				int32 CurWidgetsMaxLayerId;
				{
					/** When diffing nodes, nodes that are different between revisions are opaque, nodes that have not changed are faded */
					FGraphDiffControl::FNodeMatch NodeMatch = FGraphDiffControl::FindNodeMatch(GraphObjToDiff, NodeObj, NodeMatches);
					if (NodeMatch.IsValid())
					{
						NodeMatches.Add(NodeMatch);
					}
					const bool bNodeIsDifferent = (!GraphObjToDiff || NodeMatch.Diff(FGraphDiffControl::FNodeDiffContext()));
				
					/* When dragging off a pin, we want to duck the alpha of some nodes */
					TSharedPtr< SGraphPin > OnlyStartPin = (1 == PreviewConnectorFromPins.Num()) ? PreviewConnectorFromPins[0].FindInGraphPanel(*this) : TSharedPtr< SGraphPin >();
					const bool bNodeIsNotUsableInCurrentContext = Schema->FadeNodeWhenDraggingOffPin(NodeObj, OnlyStartPin.IsValid() ? OnlyStartPin.Get()->GetPinObj() : nullptr);
					
					const FWidgetStyle& NodeStyle = (bNodeIsDifferent && !bNodeIsNotUsableInCurrentContext)? InWidgetStyle : FadedStyle;
					FWidgetStyle NodeStyleToUse = NodeStyle;
					NodeStyleToUse.BlendColorAndOpacityTint(FLinearColor(1.0f, 1.0f, 1.0f, Alpha));
				
					// Draw the node.O
					CurWidgetsMaxLayerId = CurWidget.Widget->Paint(NewArgs, CurWidget.Geometry, MyCullingRect, OutDrawElements, ChildLayerId, NodeStyleToUse, !((SGraphPanelPublic*)this)->DisplayAsReadOnly.Get() && ShouldBeEnabled( bParentEnabled ) );
				}

				// Draw the node's overlay, if it has one.
				{
					// Get its size
					const FVector2D WidgetSize = CurWidget.Geometry.Size;

					{
						TArray<FOverlayBrushInfo> OverlayBrushes;
						ChildNode->GetOverlayBrushes(bSelected, WidgetSize, /*out*/ OverlayBrushes);

						for (int32 BrushIndex = 0; BrushIndex < OverlayBrushes.Num(); ++BrushIndex)
						{
							FOverlayBrushInfo& OverlayInfo = OverlayBrushes[BrushIndex];
							const FSlateBrush* OverlayBrush = OverlayInfo.Brush;
							if (OverlayBrush != nullptr)
							{
								FPaintGeometry BouncedGeometry = CurWidget.Geometry.ToPaintGeometry(OverlayInfo.OverlayOffset, OverlayBrush->ImageSize, 1.f);

								// Handle bouncing during PIE
								const float BounceValue = FMath::Sin(2.0f * PI * BounceCurve.GetLerp());
								BouncedGeometry.DrawPosition += (OverlayInfo.AnimationEnvelope * BounceValue * ZoomFactor);

								FLinearColor FinalColorAndOpacity(InWidgetStyle.GetColorAndOpacityTint()* OverlayBrush->GetTint(InWidgetStyle));
								//FinalColorAndOpacity.A = Alpha;

								CurWidgetsMaxLayerId++;
								FSlateDrawElement::MakeBox(
									OutDrawElements,
									CurWidgetsMaxLayerId,
									BouncedGeometry,
									OverlayBrush,
									ESlateDrawEffect::None,
									FinalColorAndOpacity
									);
							}

						}
					}

					{
						TArray<FOverlayWidgetInfo> OverlayWidgets = ChildNode->GetOverlayWidgets(bSelected, WidgetSize);

						for (int32 WidgetIndex = 0; WidgetIndex < OverlayWidgets.Num(); ++WidgetIndex)
						{
							FOverlayWidgetInfo& OverlayInfo = OverlayWidgets[WidgetIndex];
							if(OverlayInfo.Widget->GetVisibility() == EVisibility::Visible)
							{
								// call SlatePrepass as these widgets are not in the 'normal' child hierarchy
								OverlayInfo.Widget->SlatePrepass(AllottedGeometry.GetAccumulatedLayoutTransform().GetScale());

								const FGeometry WidgetGeometry = CurWidget.Geometry.MakeChild(OverlayInfo.OverlayOffset, OverlayInfo.Widget->GetDesiredSize());

								OverlayInfo.Widget->Paint(NewArgs, WidgetGeometry, MyCullingRect, OutDrawElements, CurWidgetsMaxLayerId, InWidgetStyle, bParentEnabled);
							}
						}
					}
				}

				MaxLayerId = FMath::Max( MaxLayerId, CurWidgetsMaxLayerId + 1 );
			}
		}
	}

	MaxLayerId += 1;


	// Draw connections between pins 
	if (Children.Num() > 0 )
	{
		FConnectionDrawingPolicy* ConnectionDrawingPolicy = nullptr;
		if (((SGraphPanelPublic*)this)->NodeFactory.IsValid())
		{
			ConnectionDrawingPolicy = ((SGraphPanelPublic*)this)->NodeFactory->CreateConnectionPolicy(Schema, WireLayerId, MaxLayerId, ZoomFactor, MyCullingRect, OutDrawElements, GraphObj);
		}
		else
		{
			ConnectionDrawingPolicy = FNodeFactory::CreateConnectionPolicy(Schema, WireLayerId, MaxLayerId, ZoomFactor, MyCullingRect, OutDrawElements, GraphObj);
		}

		TArray<TSharedPtr<SGraphPin>> OverridePins;
		for (const FGraphPinHandle& Handle : PreviewConnectorFromPins)
		{
			TSharedPtr<SGraphPin> Pin = Handle.FindInGraphPanel(*this);
			if (Pin.IsValid() && Pin->GetPinObj())
			{
				OverridePins.Add(Pin);
			}
		}
		ConnectionDrawingPolicy->SetHoveredPins(CurrentHoveredPins, OverridePins, TimeWhenMouseEnteredPin);
		ConnectionDrawingPolicy->SetMarkedPin(MarkedPin);
		ConnectionDrawingPolicy->SetMousePosition(AllottedGeometry.LocalToAbsolute(SavedMousePosForOnPaintEventLocalSpace));

		// Get the set of pins for all children and synthesize geometry for culled out pins so lines can be drawn to them.
		TMap<TSharedRef<SWidget>, FArrangedWidget> PinGeometries;
		TSet< TSharedRef<SWidget> > VisiblePins;
		for (int32 ChildIndex = 0; ChildIndex < Children.Num(); ++ChildIndex)
		{
			TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(Children[ChildIndex]);

			// If this is a culled node, approximate the pin geometry to the corner of the node it is within
			if (IsNodeCulled(ChildNode, AllottedGeometry) || ChildNode->IsHidingPinWidgets())
			{
				TArray< TSharedRef<SWidget> > NodePins;
				ChildNode->GetPins(NodePins);

				const FVector2D NodeLoc = ChildNode->GetPosition();
				const FGeometry SynthesizedNodeGeometry(GraphCoordToPanelCoord(NodeLoc) * AllottedGeometry.Scale, AllottedGeometry.AbsolutePosition, FVector2D::ZeroVector, 1.f);

				for (TArray< TSharedRef<SWidget> >::TConstIterator NodePinIterator(NodePins); NodePinIterator; ++NodePinIterator)
				{
					const SGraphPin& PinWidget = static_cast<const SGraphPin&>((*NodePinIterator).Get());
					if (PinWidget.GetPinObj())
					{
						FVector2D PinLoc = NodeLoc + PinWidget.GetNodeOffset();

						const FGeometry SynthesizedPinGeometry(GraphCoordToPanelCoord(PinLoc) * AllottedGeometry.Scale, AllottedGeometry.AbsolutePosition, FVector2D::ZeroVector, 1.f);
						PinGeometries.Add(*NodePinIterator, FArrangedWidget(*NodePinIterator, SynthesizedPinGeometry));
					}
				}

				// Also add synthesized geometries for culled nodes
				ArrangedChildren.AddWidget( FArrangedWidget(ChildNode, SynthesizedNodeGeometry) );
			}
			else
			{
				ChildNode->GetPins(VisiblePins);
			}
		}

		// Now get the pin geometry for all visible children and append it to the PinGeometries map
		TMap<TSharedRef<SWidget>, FArrangedWidget> VisiblePinGeometries;
		{
			this->FindChildGeometries(AllottedGeometry, VisiblePins, VisiblePinGeometries);
			PinGeometries.Append(VisiblePinGeometries);
		}

		// Draw preview connections (only connected on one end)
		if (PreviewConnectorFromPins.Num() > 0)
		{
			for (const FGraphPinHandle& Handle : PreviewConnectorFromPins)
			{
				TSharedPtr< SGraphPin > CurrentStartPin = Handle.FindInGraphPanel(*this);
				if (!CurrentStartPin.IsValid() || !CurrentStartPin->GetPinObj())
				{
					continue;
				}
				const FArrangedWidget* PinGeometry = PinGeometries.Find( CurrentStartPin.ToSharedRef() );

				if (PinGeometry != nullptr)
				{
					FVector2D StartPoint;
					FVector2D EndPoint;

					if (CurrentStartPin->GetDirection() == EGPD_Input)
					{
						StartPoint = AllottedGeometry.LocalToAbsolute(PreviewConnectorEndpoint);
						EndPoint = FGeometryHelper::VerticalMiddleLeftOf( PinGeometry->Geometry ) - FVector2D(ConnectionDrawingPolicy->ArrowRadius.X, 0);
					}
					else
					{
						StartPoint = FGeometryHelper::VerticalMiddleRightOf( PinGeometry->Geometry );
						EndPoint = AllottedGeometry.LocalToAbsolute(PreviewConnectorEndpoint);
					}

					ConnectionDrawingPolicy->DrawPreviewConnector(PinGeometry->Geometry, StartPoint, EndPoint, CurrentStartPin.Get()->GetPinObj());
				}

				//@TODO: Re-evaluate this incompatible mojo; it's mutating every pin state every frame to accomplish a visual effect
				ConnectionDrawingPolicy->SetIncompatiblePinDrawState(CurrentStartPin, VisiblePins);
			}
		}
		else
		{
			//@TODO: Re-evaluate this incompatible mojo; it's mutating every pin state every frame to accomplish a visual effect
			ConnectionDrawingPolicy->ResetIncompatiblePinDrawState(VisiblePins);
		}

		// Draw all regular connections
		ConnectionDrawingPolicy->Draw(PinGeometries, ArrangedChildren);

		// Pull back data from the drawing policy
		{
			FGraphSplineOverlapResult OverlapData = ConnectionDrawingPolicy->SplineOverlapResult;

			if (OverlapData.IsValid())
			{
				OverlapData.ComputeBestPin();

				// Only allow spline overlaps when there is no node under the cursor (unless it is a comment box)
				const FVector2D PaintAbsoluteSpaceMousePos = AllottedGeometry.LocalToAbsolute(SavedMousePosForOnPaintEventLocalSpace);
				const int32 HoveredNodeIndex = SWidget::FindChildUnderPosition(ArrangedChildren, PaintAbsoluteSpaceMousePos);
				if (HoveredNodeIndex != INDEX_NONE)
				{
					TSharedRef<SGraphNode> HoveredNode = StaticCastSharedRef<SGraphNode>(ArrangedChildren[HoveredNodeIndex].Widget);
					UEdGraphNode_Comment* CommentNode = Cast<UEdGraphNode_Comment>(HoveredNode->GetObjectBeingDisplayed());
					if (CommentNode == nullptr)
					{
						// Wasn't a comment node, disallow the spline interaction
						OverlapData = FGraphSplineOverlapResult(OverlapData.GetCloseToSpline());
					}
				}
			}

			// Update the spline hover state
			const_cast<SPMSGraphPanel*>(this)->OnSplineHoverStateChanged(OverlapData);
		}

		delete ConnectionDrawingPolicy;
	}

	// Draw a shadow overlay around the edges of the graph
	++MaxLayerId;
	PaintSurroundSunkenShadow(FEditorStyle::GetBrush(TEXT("Graph.Shadow")), AllottedGeometry, MyCullingRect, OutDrawElements, MaxLayerId);

	if (ShowGraphStateOverlay.Get())
	{
		const FSlateBrush* BorderBrush = nullptr;
		if ((GEditor->bIsSimulatingInEditor || GEditor->PlayWorld != nullptr))
		{
			// Draw a surrounding indicator when PIE is active, to make it clear that the graph is read-only, etc...
			BorderBrush = FEditorStyle::GetBrush(TEXT("Graph.PlayInEditor"));
		}
		else if (!IsEditable.Get())
		{
			// Draw a different border when we're not simulating but the graph is read-only
			BorderBrush = FEditorStyle::GetBrush(TEXT("Graph.ReadOnlyBorder"));
		}

		if (BorderBrush != nullptr)
		{
			// Actually draw the border
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				MaxLayerId,
				AllottedGeometry.ToPaintGeometry(),
				BorderBrush
				);
		}
	}

	// Draw the marquee selection rectangle
	PaintMarquee(AllottedGeometry, MyCullingRect, OutDrawElements, MaxLayerId);

	// Draw the software cursor
	++MaxLayerId;
	PaintSoftwareCursor(AllottedGeometry, MyCullingRect, OutDrawElements, MaxLayerId);

	return MaxLayerId;
}

FReply SPMSGraphPanel::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	//LastKeyChordDetected;
	return SGraphPanel::OnKeyDown(MyGeometry, InKeyEvent);
}

bool SPMSGraphPanel::OnHandleLeftMouseRelease(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return SGraphPanel::OnHandleLeftMouseRelease(MyGeometry, MouseEvent);
}

void SPMSGraphPanel::OnSplineHoverStateChanged(const FGraphSplineOverlapResult& NewSplineHoverState)
{
	TSharedPtr<SGraphPin> OldPinWidget = PreviousFrameSplineOverlap.GetBestPinWidget(*this);
	PreviousFrameSplineOverlap = NewSplineHoverState;
	TSharedPtr<SGraphPin> NewPinWidget = PreviousFrameSplineOverlap.GetBestPinWidget(*this);

	PreviousFrameSavedMousePosForSplineOverlap = SavedMousePosForOnPaintEventLocalSpace;

	// Handle mouse enter/leaves on the associated pin
	if (OldPinWidget != NewPinWidget)
	{
		if (OldPinWidget.IsValid())
		{
			OldPinWidget->OnMouseLeave(LastPointerEvent);
		}

		if (NewPinWidget.IsValid())
		{
			NewPinWidget->OnMouseEnter(LastPointerGeometry, LastPointerEvent);

			// Get the pin/wire glowing quicker, since it's a direct selection (this time was already set to 'now' as part of entering the pin)
			//@TODO: Source this parameter from the graph rendering settings once it is there (see code in ApplyHoverDeemphasis)
			TimeWhenMouseEnteredPin -= 0.75f;
		}
	}
}
inline float FancyMod(float Value, float Size)
{
	return ((Value >= 0) ? 0.0f : Size) + FMath::Fmod(Value, Size);
}
void SPMSGraphPanel::PaintBackgroundAsLines(const FSlateBrush* BackgroundImage, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32& DrawLayerId) const
{
	const bool bAntialias = false;

	const int32 RulePeriod = GetDefault<UPMSEditorSettings>()->SubGridCount;
	check(RulePeriod > 0);

	const FLinearColor GraphBackGroundImageColor(BackgroundImage->TintColor.GetSpecifiedColor());
	const FLinearColor RegularColor(GetDefault<UEditorStyleSettings>()->RegularColor);
	const FLinearColor RuleColor(GetDefault<UEditorStyleSettings>()->RuleColor);
	const FLinearColor CenterColor(GetDefault<UEditorStyleSettings>()->CenterColor);
	const float GraphSmallestGridSize = 8.0f;
	const float RawZoomFactor = GetZoomAmount();
	const float NominalGridSize = GetDefault<UPMSEditorSettings>()->GridSize/RulePeriod;

	float ZoomFactor = RawZoomFactor;
	float Inflation = 1.0f;
	while (ZoomFactor*Inflation*NominalGridSize <= GraphSmallestGridSize)
	{
		Inflation *= 2.0f;
	}
	
	const float GridCellSize = NominalGridSize * ZoomFactor * Inflation;

	const float GraphSpaceGridX0 = FancyMod(ViewOffset.X, Inflation * NominalGridSize * RulePeriod);
	const float GraphSpaceGridY0 = FancyMod(ViewOffset.Y, Inflation * NominalGridSize * RulePeriod);

	float ImageOffsetX = GraphSpaceGridX0 * -ZoomFactor;
	float ImageOffsetY = GraphSpaceGridY0 * -ZoomFactor;

	const FVector2D ZeroSpace = GraphCoordToPanelCoord(FVector2D::ZeroVector);
	
	// Fill the background
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		DrawLayerId,
		AllottedGeometry.ToPaintGeometry(),
		BackgroundImage,
		ESlateDrawEffect::None,
		GraphBackGroundImageColor
	);

	TArray<FVector2D> LinePoints;
	new (LinePoints) FVector2D(0.0f, 0.0f);
	new (LinePoints) FVector2D(0.0f, 0.0f);

	//If we want to use grid then show grid, otherwise don't render the grid
	if (GetDefault<UEditorStyleSettings>()->bUseGrid == true){

		// Horizontal bars
		for (int32 GridIndex = 0; ImageOffsetY < AllottedGeometry.GetLocalSize().Y; ImageOffsetY += GridCellSize, ++GridIndex)
		{
			if (ImageOffsetY >= 0.0f)
			{
				const bool bIsRuleLine = (GridIndex % RulePeriod) == 0;
				const int32 Layer = bIsRuleLine ? (DrawLayerId + 1) : DrawLayerId;

				const FLinearColor* Color = bIsRuleLine ? &RuleColor : &RegularColor;
				if (FMath::IsNearlyEqual(ZeroSpace.Y, ImageOffsetY, 1.0f))
				{
					Color = &CenterColor;
				}

				LinePoints[0] = FVector2D(0.0f, ImageOffsetY);
				LinePoints[1] = FVector2D(AllottedGeometry.GetLocalSize().X, ImageOffsetY);

				FSlateDrawElement::MakeLines(
					OutDrawElements,
					Layer,
					AllottedGeometry.ToPaintGeometry(),
					LinePoints,
					ESlateDrawEffect::None,
					*Color,
					bAntialias);
			}
		}

		// Vertical bars
		for (int32 GridIndex = 0; ImageOffsetX < AllottedGeometry.GetLocalSize().X; ImageOffsetX += GridCellSize, ++GridIndex)
		{
			if (ImageOffsetX >= 0.0f)
			{
				const bool bIsRuleLine = (GridIndex % RulePeriod) == 0;
				const int32 Layer = bIsRuleLine ? (DrawLayerId + 1) : DrawLayerId;

				const FLinearColor* Color = bIsRuleLine ? &RuleColor : &RegularColor;
				if (FMath::IsNearlyEqual(ZeroSpace.X, ImageOffsetX, 1.0f))
				{
					Color = &CenterColor;
				}

				LinePoints[0] = FVector2D(ImageOffsetX, 0.0f);
				LinePoints[1] = FVector2D(ImageOffsetX, AllottedGeometry.GetLocalSize().Y);

				FSlateDrawElement::MakeLines(
					OutDrawElements,
					Layer,
					AllottedGeometry.ToPaintGeometry(),
					LinePoints,
					ESlateDrawEffect::None,
					*Color,
					bAntialias);
			}
		}
	}
	DrawLayerId += 2;
}


class SGraphPin* SPMSGraphPanel::GetBestPinFromHoveredSpline() const
{
	TSharedPtr<SGraphPin> BestPinWidget = PreviousFrameSplineOverlap.GetBestPinWidget(*this);
	return BestPinWidget.Get();
}

bool SPMSGraphPanel::PassesAssetReferenceFilter(const TArray<FAssetData>& ReferencedAssets, FText* OutFailureReason) const
{
	if (GEditor)
	{
		FAssetReferenceFilterContext AssetReferenceFilterContext;
		UObject* GraphOuter = GraphObj ? GraphObj->GetOuter() : nullptr;
		if (GraphOuter)
		{
			AssetReferenceFilterContext.ReferencingAssets.Add(FAssetData(GraphOuter));
		}
		TSharedPtr<IAssetReferenceFilter> AssetReferenceFilter = GEditor->MakeAssetReferenceFilter(AssetReferenceFilterContext);
		if (AssetReferenceFilter.IsValid())
		{
			for (const FAssetData& Asset : ReferencedAssets)
			{
				if (!AssetReferenceFilter->PassesFilter(Asset, OutFailureReason))
				{
					return false;
				}
			}
		}
	}

	return true;
}