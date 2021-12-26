// Fill out your copyright notice in the Description page of Project Settings.


#pragma once
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"
#include "CoreMinimal.h"
#include "Editor/PMSEditorSettings.h"
#include "Editor/SlateWidgets/HackPrivate/SGraphPanelPublic.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor/GraphEditor/Private/DragConnection.h"
#include "Editor/GraphEditor/Private/SGraphEditorImpl.h"
#include "Editor/SlateWidgets/SPMSEdGraphNode.h"
#include "Editor/SlateWidgets/SPMSEdGraphPin.h"
#include "Editor/Utilities/PMSEdGraphCommand.h"
#include "Editor/Utilities/PMSEditorUtilities.h"
//#include "SGraphEditorImpl.h"

#define UpdateMoveTogetherNodesPos(MoveTogetherNodes,MoveTogetherNodesStartPos,UNodeBeingDrag,DragStartPos)\
for(int i=0;i<MoveTogetherNodes.Num();i++)\
{\
	UPMSEdGraphNode* MoveTogetherNode = MoveTogetherNodes[i];\
	FVector2D CurNodeStartPos = MoveTogetherNodesStartPos[i];\
	MoveTogetherNode->NodePosX = CurNodeStartPos.X + UNodeBeingDrag->NodePosX - DragStartPos.X;\
	MoveTogetherNode->NodePosY = CurNodeStartPos.Y + UNodeBeingDrag->NodePosY - DragStartPos.Y;\
}

FPMSEdGraphPanelInputPreProcessor::FPMSEdGraphPanelInputPreProcessor()
{
	ShakeOffNodeTracker.Reserve(3);
	HighLightedNodes.Reserve(10);
	HighLightedPins.Reserve(10);

	FPMSEdGraphCommands::Register();
	UICommandList = MakeShared<FUICommandList>();
	const FPMSEdGraphCommands& Commands = FPMSEdGraphCommands::Get();
	// UICommandList->MapAction(Commands.DownStreamNodesFollowMove,FExecuteAction::CreateRaw(this,&PMSEdGraphPaneInputPreProcessor::OnSelectLinkedNodes,true,false));
	// UICommandList->MapAction(Commands.UpStreamNodesFollowMove,FExecuteAction::CreateRaw(this,&PMSEdGraphPaneInputPreProcessor::OnSelectLinkedNodes,false,true));
	
	// TSharedPtr< FUICommandInfo > RearrangeNode;
	// TSharedPtr< FUICommandInfo > BypassNodes;
	// TSharedPtr< FUICommandInfo > CycleWireDrawStyle;
	// TSharedPtr< FUICommandInfo > ToggleMaterialGraphWireColor;
	//TSharedPtr< FUICommandInfo > ToggleAutoConnect;
	// TSharedPtr< FUICommandInfo > DuplicateNodeWithInput;
	// TSharedPtr< FUICommandInfo > ExchangeWires;
	// TSharedPtr< FUICommandInfo > ToggleInsertNode;
	// TSharedPtr< FUICommandInfo > BypassAndKeepNodes;
	// TSharedPtr< FUICommandInfo > ConnectNodes;
}
FPMSEdGraphPanelInputPreProcessor::~FPMSEdGraphPanelInputPreProcessor()
{
	const FPMSEdGraphCommands& Commands = FPMSEdGraphCommands::Get();
	//UICommandList->UnmapAction(Commands.DownStreamNodesFollowMove);
	//UICommandList->UnmapAction(Commands.UpStreamNodesFollowMove);
	UICommandList.Reset();
	FPMSEdGraphCommands::Unregister();
}

void FPMSEdGraphPanelInputPreProcessor::UpdateEventContext(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	FPMSEventContex ctx;
	//CurContext.IsDraggingConnection = SlateApp.GetDragDroppingContent()->IsOfType<FDragConnection>();
	
	FWidgetPath widgetsUnderCursor = SlateApp.LocateWindowUnderMouse(MouseEvent.GetScreenSpacePosition(), SlateApp.GetInteractiveTopLevelWindows());
	//Todo 为什么此处要构造这个？
	FScopedSwitchWorldHack SwitchWorld(widgetsUnderCursor);
	bool bHasGetNode = false;
	bool bHasGetFlag = false;
	bool bHasGetPin = false;
	for(int i= widgetsUnderCursor.Widgets.Num()-1;i>=0;i--)
	{
		const auto curwidget = widgetsUnderCursor.Widgets[i];
		FString widgetName = curwidget.Widget->GetTypeAsString();
		if (widgetName == "SPMSGraphPanel" && GetDefault<UPMSEditorSettings>()->EnablePreProcesser)
		{
			TSharedPtr<SGraphPanel> GraphPanel = StaticCastSharedRef<SGraphPanel>(widgetsUnderCursor.Widgets[i].Widget);
			if(GraphPanel->GetGraphObj()->GetClass()->GetFName().ToString() == "PMSEdGraph")
			{
				ctx.IsCursorInsidePanel = true;
				ctx.PanelGeometry = curwidget.Geometry;
				ctx.GraphPanel = GraphPanel;
				/*如果鼠标下最顶上的widget就是graphpanel，则是在grpahpanel的emptyspace*/
				if (i == widgetsUnderCursor.Widgets.Num()-1)
				{
					ctx.IsCursorOnPanelEmptySpace = true;
				}
			}
		}

		//Todo 也许bHasGetFlag此类变量没有必要创建，因为在widgetsUnderCursor中默认是capture最上层后返回整个链
		/* Get top nodeflag widget under cursor*/
		if(!bHasGetFlag && widgetName.Contains("NodeFlag"))
		{
			ctx.NodeFlag = StaticCastSharedRef<SNodeFlagCheckBox>(curwidget.Widget);
			ctx.NodeFlagGeometry = curwidget.Geometry;
			bHasGetFlag = true;
		}
		/* Get top node widget under cursor*/
		if(!bHasGetNode && widgetName.Contains("PMSEdGraphNode"))
		{
			ctx.GraphNode = StaticCastSharedRef<SPMSEdGraphNode>(curwidget.Widget);
			ctx.NodeGeometry = curwidget.Geometry;
			bHasGetNode = true;
		}
		/* Get top pin widget under cursor*/
		if(!bHasGetPin && widgetName.Contains("PMSEdGraphPin"))
		{
			//Todo SPMSEdGraphPin在编辑器中存在奇怪的位置偏移
			ctx.GraphPin = StaticCastSharedRef<SPMSEdGraphPin>(curwidget.Widget);
			ctx.PinGeometry = curwidget.Geometry;
			bHasGetPin = true;
		}
	}
	CurContext = ctx;
}

void FPMSEdGraphPanelInputPreProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp,
	TSharedRef<ICursor> Cursor)
{
	MouseUpDeltaTime += DeltaTime;

	for (int i = 0; i < TickEventListener.Num(); i++)
	{
		if (!TickEventListener[i].IsBound() || (TickEventListener[i].IsBound() && TickEventListener[i].Execute()))
		{
			TickEventListener.RemoveAt(i);
			i--;
		}
	}
}

bool FPMSEdGraphPanelInputPreProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	if(MouseEnterState == EMouseEnterState::None)
	{
		UpdateEventContext(SlateApp, MouseEvent);		
		LastMouseDownScreenPos = MouseEvent.GetScreenSpacePosition();

		/* Update ContextEnterState*/
		if (CurContext.IsCursorInsidePanel)
		{
			LastGraphCursorGraphPos =  ScreenPosToGraphPos(CurContext.GraphPanel.ToSharedRef(),CurContext.PanelGeometry,LastMouseDownScreenPos);
			if (!SlateApp.HasAnyMouseCaptor())
			{
				if (CurContext.GraphNode.IsValid() || CurContext.CommentNode.IsValid())
				{
					if (SlateApp.GetPlatformCursor().IsValid())
					{
						//when cursor is CardinalCross,its draggable part of the node
						auto cursorIcon = SlateApp.GetPlatformCursor()->GetType();
						if (!CurContext.NodeFlag.IsValid() && (cursorIcon == EMouseCursor::CardinalCross || cursorIcon == EMouseCursor::Default))
						{
							NodeBeingDrag = CurContext.GraphNode;
							CommentNodeBeingDrag = CurContext.CommentNode;
							ContextEnterState = EContextEnterState::OnNode;
						}
						else
						{
							ContextEnterState = EContextEnterState::OnPin;
						}
					}
				}
				else
				{
					ContextEnterState = EContextEnterState::OnSpace;
				}
			}
		}
		
		{
			if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
            {
            	MouseEnterState = EMouseEnterState::Left;		
            }
            else if(MouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
            {
            	MouseEnterState = EMouseEnterState::Middle;
            }
            else if(MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
            {
            	MouseEnterState = EMouseEnterState::Right;
            }
            else
            {
            	MouseEnterState = EMouseEnterState::None;
            }
		}
				
		if(ContextEnterState == EContextEnterState::OnNode)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				/* ModifierKeys store state of ctrl, shift and alt, we don't need to store these in this class */
				const bool CtrlState = MouseEvent.GetModifierKeys().IsControlDown();
				const bool ShiftState = MouseEvent.GetModifierKeys().IsShiftDown();		
				const FGraphPanelSelectionSet SelectedNodes = CurContext.GraphPanel->SelectionManager.SelectedNodes;
				UPMSEdGraphNode* EnterNode = NodeBeingDrag.Pin()->GetPMSNodeObj();
			
				UpdateMoveTogetherNodes(EnterNode,SelectedNodes,CtrlState,ShiftState);
				// for(UPMSEdGraphNode* MoveTogetherNode:MoveTogetherNodes)
				// {
				// 	//MoveTogetherNodesStartPos.Add(FVector2D(MoveTogetherNode->NodePosX,MoveTogetherNode->NodePosY));
				// }		
			
				DragNodeStartPos = FVector2D(EnterNode->NodePosX,EnterNode->NodePosY);
				MouseMovementAfterDown = FVector2D::ZeroVector;
				return true;
			}
		}

		if(ContextEnterState == EContextEnterState::OnSpace)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				return false;
			}
			if(MouseEnterState == EMouseEnterState::Middle)
			{
				SGraphPanelPublic* Temp = (SGraphPanelPublic*)CurContext.GraphPanel.Get();
				DragViewStartPos = Temp->ViewOffset;
				return true;
			}
			if(MouseEnterState == EMouseEnterState::Right)
			{
				return true;
			}
		}
		
		return IInputProcessor::HandleMouseButtonDownEvent(SlateApp, MouseEvent);	
	}
	/*Block all mouse button down event when already down*/
	return true;
	
}

bool FPMSEdGraphPanelInputPreProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	/* Todo 节点还是不跟手，拖动会有延迟 在处理鼠标移动事件时 FSlateApplication::ProcessMouseMoveEvent的确是优先处理
	 * PreProcessor再向下传递，估计是后续在SGraphPanel中有逻辑处理更新
	 */
	
	UpdateEventContext(SlateApp, MouseEvent);
	if(ContextEnterState!=EContextEnterState::None)
	{
		bMoveBeforeUp = true;
	}
	FVector2D ScreenCurPos = MouseEvent.GetScreenSpacePosition();
	FVector2D ScreenPrvPos = MouseEvent.GetLastScreenSpacePosition();
	
	if(CurContext.IsCursorInsidePanel)
	{
		FVector2D GraphCurPos = ScreenPosToGraphPos(CurContext.GraphPanel.ToSharedRef(),CurContext.PanelGeometry,ScreenCurPos);
		//LastGraphCursorGraphPos =  ScreenPosToGraphPos(CurContext.GraphPanel.ToSharedRef(),CurContext.PanelGeometry,LastMouseDownScreenPos);
		MouseMovementAfterDown = GraphCurPos-LastGraphCursorGraphPos;
		
		if(ContextEnterState == EContextEnterState::OnNode)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				UPMSEdGraphNode* EnterNode = NodeBeingDrag.Pin()->GetPMSNodeObj();
				/*Get NodePos before Move*/
				FVector2D NodePosTemp = FVector2D(EnterNode->NodePosX,EnterNode->NodePosY);
				EnterNode->NodePosX = DragNodeStartPos.X + MouseMovementAfterDown.X;
				EnterNode->NodePosY = DragNodeStartPos.Y + MouseMovementAfterDown.Y;
				/**/
				FChildren* Children = CurContext.GraphPanel->GetChildren();
				TArray<FVector2D> PossibleSnapPosArray;
				//GetChildRefAt(Index).GetWidget();
				Children->ForEachWidget([&PossibleSnapPosArray](SWidget& Child)
				{
					// SPMSEdGraphNode* PMSChild = (SPMSEdGraphNode*)&Child;
					// FString Name = PMSChild->GetPMSNodeObj()->IconName;
					// FVector2D Pivot = Child.GetRenderTransformPivot();
					// UE_LOG(LogTemp,Log,TEXT("Node is %s:%f,%f"),ToCStr(Name),Pivot.X,Pivot.Y);
					UPMSEdGraphNode* PMSChild = ((SPMSEdGraphNode*)&Child)->GetPMSNodeObj();
					if(!PMSChild->AlreadyMoveTogether)
					{
						PossibleSnapPosArray.Add(FVector2D(PMSChild->NodePosX,PMSChild->NodePosY));
					}
				});
				/*Move NodeBeingDrag with snap*/
				EnterNode->PMSSnapToGrid(GetDefault<UPMSEditorSettings>()->GridSize,GetDefault<UPMSEditorSettings>()->SnappingDistance,PossibleSnapPosArray);
			
				/*Move MoveTogetherNodes*/
				UpdateMoveTogetherNodesPos(MoveTogetherNodes,MoveTogetherNodesStartPos,EnterNode,DragNodeStartPos);

				/*Move Selection*/
				const FGraphPanelSelectionSet SelectedNodes = CurContext.GraphPanel->SelectionManager.SelectedNodes;
				if(SelectedNodes.Find(EnterNode))
				{
					for(UObject* SelectedNode: SelectedNodes)
					{
						if(SelectedNode!=EnterNode)
						{
							Cast<UPMSEdGraphNode>(SelectedNode)->NodePosX += EnterNode->NodePosX - NodePosTemp.X;
							Cast<UPMSEdGraphNode>(SelectedNode)->NodePosY += EnterNode->NodePosY - NodePosTemp.Y;					
						}				
					}	
				}
			}
		}
		if(ContextEnterState == EContextEnterState::OnSpace)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				
			}
			if(MouseEnterState == EMouseEnterState::Middle)
			{
				SGraphPanelPublic* Temp = (SGraphPanelPublic*)CurContext.GraphPanel.Get();
				Temp->ViewOffset = DragViewStartPos - MouseMovementAfterDown;
				DragViewStartPos -= MouseMovementAfterDown;
			}
			if(MouseEnterState == EMouseEnterState::Right)
			{
				
			}
		}
	}
	
	
	// else
	// {
	// 	return false;
	// }
	
	return IInputProcessor::HandleMouseMoveEvent(SlateApp, MouseEvent);
}

bool FPMSEdGraphPanelInputPreProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	EMouseEnterState MouseLeaveState = EMouseEnterState::None;
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		MouseLeaveState = EMouseEnterState::Left;		
	}
	else if(MouseEvent.GetEffectingButton() == EKeys::MiddleMouseButton)
	{
		MouseLeaveState = EMouseEnterState::Middle;
	}
	else if(MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		MouseLeaveState = EMouseEnterState::Right;
	}
	// else
	// {
	// 	MouseLeaveState = EMouseEnterState::None;
	// }

	if(MouseEnterState == MouseLeaveState)
	{
		
		UpdateEventContext(SlateApp, MouseEvent);
		bool BlockNextStep = false;
		bool ReturnType = false;
	
		float deltaTime = MouseUpDeltaTime;
		MouseUpDeltaTime = 0.f;
		float deltaPos = (LastMouseUpScreenPos - MouseEvent.GetScreenSpacePosition()).Size();
		LastMouseUpScreenPos = MouseEvent.GetScreenSpacePosition();
		if (deltaTime < 0.5f && deltaPos < 5)
		{
			CurContext.IsDoubleClickGesture = true;
		}
		if(ContextEnterState == EContextEnterState::OnNode)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				if(!bMoveBeforeUp)
				{
					CurContext.GraphPanel->SelectionManager.SelectedNodes.Reset();
					CurContext.GraphPanel->SelectionManager.SelectedNodes.Add(NodeBeingDrag.Pin()->GetPMSNodeObj());
				}
				const FGraphPanelSelectionSet SelectedNodes = CurContext.GraphPanel->SelectionManager.SelectedNodes;
				UPMSEdGraphNode* EnterNode = NodeBeingDrag.Pin()->GetPMSNodeObj();
				
				if(SelectedNodes.Find(EnterNode))
				{		
					if(CurContext.GraphPanel.IsValid())
					{
						for(UObject* Node : CurContext.GraphPanel->SelectionManager.GetSelectedNodes())
						{
							Cast<UPMSEdGraphNode>(Node)->AlreadyMoveTogether = false;
						}
		
					}
				}
				else
				{
					EnterNode->AlreadyMoveTogether = false;
				}
				
				for(UPMSEdGraphNode* MoveTogetherNode:MoveTogetherNodes)
				{
					MoveTogetherNode->AlreadyMoveTogether = false;
					/*This is possibly not necessary*/
					MoveTogetherNode->StillMoveTogether = false;
				}
				
				NodeBeingDrag.Reset();
				MoveTogetherNodes.Reset();
				MoveTogetherNodesStartPos.Reset();				
		
				DragNodeStartPos = FVector2D::ZeroVector;
				MouseMovementAfterDown = FVector2D::ZeroVector;
		
				bMoveBeforeUp = false;
				ReturnType = true;			
			}
		}
		if(ContextEnterState == EContextEnterState::OnSpace)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				ReturnType = false;
			}
			if(MouseEnterState == EMouseEnterState::Middle)
			{
				ReturnType = true;
			}
			if(MouseEnterState == EMouseEnterState::Right)
			{
				if(!bMoveBeforeUp)
				{
					ReturnType = false;
				}
				else
				{
					bMoveBeforeUp = false;
					ReturnType = true;
				}
			}
		}
	
		MouseEnterState = EMouseEnterState::None;
		ContextEnterState = EContextEnterState::None;
		bMoveBeforeUp = false;
		return ReturnType;
	}
	/*Block all mouse button up event if doesn't match current mouse enter event*/
	return true;
}

bool FPMSEdGraphPanelInputPreProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	//FModifierKeysState State = StaticCast<FInputEvent>(InKeyEvent).ModifierKeys;
	//FModifierKeysState();
	const bool CtrlState = InKeyEvent.IsControlDown();
	const bool ShiftState = InKeyEvent.IsShiftDown();
	const bool AltState = InKeyEvent.IsAltDown();

	bool BlockNextStep = false;
	bool ReturnType = false;
	
	UE_LOG(LogTemp, Log, TEXT("Node Name is %s"), ToCStr(InKeyEvent.ToText().ToString()));
	if(CtrlState!=PrvCtrlState || ShiftState!=PrvShiftState)
	{
		if(ContextEnterState == EContextEnterState::OnNode)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				const FGraphPanelSelectionSet SelectedNodes = CurContext.GraphPanel->SelectionManager.SelectedNodes;
				UPMSEdGraphNode* EnterNode = NodeBeingDrag.Pin()->GetPMSNodeObj();

				UpdateMoveTogetherNodes(EnterNode,SelectedNodes,CtrlState,ShiftState);
				UpdateMoveTogetherNodesPos(MoveTogetherNodes,MoveTogetherNodesStartPos,EnterNode,DragNodeStartPos);
			
				ReturnType = true;
			}
		}
	}
	PrvCtrlState = CtrlState;
	PrvShiftState = ShiftState;
	PrvAltState = AltState;

	return ReturnType;
	/*Todo 是否需要在这里加上这部分才可以让UICommandMap起作用？？
	if (IsCursorInsidePanel() && UICommandList->ProcessCommandBindings(InKeyEvent.GetKey(), FSlateApplication::Get().GetModifierKeys(), InKeyEvent.IsRepeat()))
	{
		return true;
	}
	*/
	
	// if(InKeyEvent.GetCharacter() > 0 )
	// {
	// 	PressedCharKey = InKeyEvent.GetCharacter();
	// 	auto Get = InKeyEvent.GetKey();
	// 	if(Get == EKeys::RightAlt)
	// 	{
	// 		return true;
	// 	}
	// }
	// return false;
	//PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,	bDownStream, bUpStream)
}

bool FPMSEdGraphPanelInputPreProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	//UE_LOG(LogTemp, Log, TEXT("Node Name is %s"), InKeyEvent.GetKey().GetDisplayName().ToString());
	const bool CtrlState = InKeyEvent.IsControlDown();
	const bool ShiftState = InKeyEvent.IsShiftDown();
	const bool AltState = InKeyEvent.IsAltDown();

	bool BlockNextStep = false;
	bool ReturnType = false;
	UE_LOG(LogTemp, Log, TEXT("Node Name is %s"), ToCStr(InKeyEvent.ToText().ToString()));
	
	// if(InKeyEvent.GetKey() == EKeys::LeftShift || InKeyEvent.GetKey() == EKeys::RightShift
	// 	|| InKeyEvent.GetKey() == EKeys::LeftControl || InKeyEvent.GetKey() == EKeys::RightControl)
	if(CtrlState!=PrvCtrlState || ShiftState!=PrvShiftState)
	{
		if(ContextEnterState == EContextEnterState::OnNode)
		{
			if(MouseEnterState == EMouseEnterState::Left)
			{
				const FGraphPanelSelectionSet SelectedNodes = CurContext.GraphPanel->SelectionManager.SelectedNodes;
				UPMSEdGraphNode* EnterNode = NodeBeingDrag.Pin()->GetPMSNodeObj();

				UpdateMoveTogetherNodes(EnterNode,SelectedNodes,CtrlState,ShiftState);
			
				ReturnType = true;
			}
		}
	}
	PrvCtrlState = CtrlState;
	PrvShiftState = ShiftState;
	PrvAltState = AltState;

	return ReturnType;
}

bool FPMSEdGraphPanelInputPreProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	UpdateEventContext(SlateApp, MouseEvent);
	if(CurContext.IsCursorInsidePanel)
	{
		return true;
	}
	return IInputProcessor::HandleMouseButtonDoubleClickEvent(SlateApp, MouseEvent);
}

bool FPMSEdGraphPanelInputPreProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp,	const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	// FVector2D ScreenCurPos = InWheelEvent.GetScreenSpacePosition();	
	// if(CurContext.IsCursorInsidePanel)
	// {
	// 	FVector2D GraphCurPos = ScreenPosToGraphPos(CurContext.GraphPanel.ToSharedRef(),CurContext.PanelGeometry,ScreenCurPos);
	// }
	return IInputProcessor::HandleMouseWheelOrGestureEvent(SlateApp, InWheelEvent, InGestureEvent);
}

TSharedPtr<SGraphPanel> FPMSEdGraphPanelInputPreProcessor::GetCurrentGraphPanel()
{
	FSlateApplication& slateApp = FSlateApplication::Get();
	FWidgetPath widgetsUnderCursor = slateApp.LocateWindowUnderMouse(slateApp.GetCursorPos(), slateApp.GetInteractiveTopLevelWindows());
	FScopedSwitchWorldHack SwitchWorld(widgetsUnderCursor);
	for (int i = widgetsUnderCursor.Widgets.Num() - 1; i >= 0; i--)
	{
		if (widgetsUnderCursor.Widgets[i].Widget->GetTypeAsString() == "SGraphPanel")
		{
			return TSharedPtr<SGraphPanel>(StaticCastSharedRef<SGraphPanel>(widgetsUnderCursor.Widgets[i].Widget));
		}
	}
	return nullptr;
}

FVector2D FPMSEdGraphPanelInputPreProcessor::GraphPosToScreenPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D PanelPos)
{
	PanelPos = (PanelPos - GraphPanel->GetViewOffset()) * GraphPanel->GetZoomAmount();
	return Geometry.LocalToAbsolute(PanelPos);
}


FVector2D FPMSEdGraphPanelInputPreProcessor::ScreenPosToGraphPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D ScreenPos)
{
	auto ZoomStartOffset = Geometry.AbsoluteToLocal(ScreenPos);
	return GraphPanel->PanelCoordToGraphCoord(ZoomStartOffset);
}

void FPMSEdGraphPanelInputPreProcessor::OnSelectLinkedNodes(bool bDownStream, bool bUpStream)
{
	
	//PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,	bDownStream, bUpStream)
}

void FPMSEdGraphPanelInputPreProcessor::UpdateMoveTogetherNodes(UPMSEdGraphNode* EnterNode, FGraphPanelSelectionSet SelectedNodes, bool CtrlState, bool ShiftState)
{
	TArray<UPMSEdGraphNode*> SelectSourceNodes;
	if(SelectedNodes.Find(EnterNode))
	{		
		if(CurContext.GraphPanel.IsValid())
		{
			for(UObject* Node : CurContext.GraphPanel->SelectionManager.GetSelectedNodes())
			{
				SelectSourceNodes.Add(Cast<UPMSEdGraphNode>(Node));
			}
		
		}
	}
	else
	{
		SelectSourceNodes.Add(EnterNode);
	}
	TArray<UPMSEdGraphNode*> TempMoveTogetherNodes = PMSEditorUtilities::GetNodesByConnectivity(SelectSourceNodes,	CtrlState, ShiftState);
	TArray<UPMSEdGraphNode*> NewMoveTogetherNodes;
	TArray<FVector2D> NewMoveTogetherNodesStartPos;
	//NewMoveTogetherNodes.InsertZeroed(TempMoveTogetherNodes.Num());
	for(int i=0;i<MoveTogetherNodes.Num();i++)
	{
		UPMSEdGraphNode* PrvMoveTogetherNode =  MoveTogetherNodes[i];
		FVector2D CurNodeStartPos = MoveTogetherNodesStartPos[i];
		if(PrvMoveTogetherNode->StillMoveTogether)
		{
			NewMoveTogetherNodes.Add(PrvMoveTogetherNode);
			NewMoveTogetherNodesStartPos.Add(CurNodeStartPos);
		}
		else
		{
			PrvMoveTogetherNode->AlreadyMoveTogether = false;
			PrvMoveTogetherNode->StillMoveTogether = false;
			PrvMoveTogetherNode->NodePosX = CurNodeStartPos.X;
			PrvMoveTogetherNode->NodePosY = CurNodeStartPos.Y;
		}
	}
	for(UPMSEdGraphNode* TempMoveTogetherNode:TempMoveTogetherNodes)
	{
		if(!TempMoveTogetherNode->StillMoveTogether)
		{
			NewMoveTogetherNodes.Add(TempMoveTogetherNode);
			NewMoveTogetherNodesStartPos.Add(FVector2D(TempMoveTogetherNode->NodePosX,TempMoveTogetherNode->NodePosY));
		}
		TempMoveTogetherNode->StillMoveTogether = false;
	}
	MoveTogetherNodes.Reset();
	MoveTogetherNodesStartPos.Reset();
	MoveTogetherNodes = MoveTemp(NewMoveTogetherNodes);
	MoveTogetherNodesStartPos = MoveTemp(NewMoveTogetherNodesStartPos);
}