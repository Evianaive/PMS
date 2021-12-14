// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "Editor/Utilities/PMSEdGraphPaneInputPreProcessor.h"
#include "CoreMinimal.h"
#include "SGraphPanel.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor/GraphEditor/Private/DragConnection.h"
#include "Editor/GraphEditor/Private/SGraphEditorImpl.h"
#include "Editor/SlateWidgets/SPMSEdGraphNode.h"
#include "Editor/SlateWidgets/SPMSEdGraphPin.h"
#include "Editor/Utilities/PMSEdGraphCommand.h"
#include "Editor/Utilities/PMSEditorUtilities.h"
//#include "SGraphEditorImpl.h"
FPMSEdGraphPaneInputPreProcessor::FPMSEdGraphPaneInputPreProcessor()
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
FPMSEdGraphPaneInputPreProcessor::~FPMSEdGraphPaneInputPreProcessor()
{
	const FPMSEdGraphCommands& Commands = FPMSEdGraphCommands::Get();
	//UICommandList->UnmapAction(Commands.DownStreamNodesFollowMove);
	//UICommandList->UnmapAction(Commands.UpStreamNodesFollowMove);
	UICommandList.Reset();
	FPMSEdGraphCommands::Unregister();
}

void FPMSEdGraphPaneInputPreProcessor::UpdateEventContext(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
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
		if (widgetName == "SGraphPanel")
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

		if(!bHasGetFlag && widgetName.Contains("NodeFlag"))
		{
			ctx.NodeFlag = StaticCastSharedRef<SNodeFlagCheckBox>(curwidget.Widget);
			ctx.NodeFlagGeometry = curwidget.Geometry;
			bHasGetFlag = true;
		}
		if(!bHasGetNode && widgetName.Contains("PMSEdGraphNode"))
		{
			ctx.GraphNode = StaticCastSharedRef<SPMSEdGraphNode>(curwidget.Widget);
			ctx.NodeGeometry = curwidget.Geometry;
			bHasGetNode = true;
		}
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

void FPMSEdGraphPaneInputPreProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp,
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

bool FPMSEdGraphPaneInputPreProcessor::HandleMouseButtonUpEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	UpdateEventContext(SlateApp, MouseEvent);
	float deltaTime = MouseUpDeltaTime;
	MouseUpDeltaTime = 0.f;
	float deltaPos = (LastMouseUpScreenPos - MouseEvent.GetScreenSpacePosition()).Size();
	LastMouseUpScreenPos = MouseEvent.GetScreenSpacePosition();
	if (deltaTime < 0.5f && deltaPos < 5)
	{
		CurContext.IsDoubleClickGesture = true;
	}
	return IInputProcessor::HandleMouseButtonUpEvent(SlateApp, MouseEvent);
}

bool FPMSEdGraphPaneInputPreProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	// if(CurContext.IsCursorInsidePanel)
	// {
	// 	return true;		
	// }
	
	LastMouseDownScreenPos = MouseEvent.GetScreenSpacePosition();
	UpdateEventContext(SlateApp, MouseEvent);

	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		//if (CurContext.IsCursorInsidePanel && !CurContext.IsDraggingConnection)
		if (CurContext.IsCursorInsidePanel)
		{
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
						}
					}
				}
			}
		}
	}
	//Todo 绘制一个事件处理的流程图来设计运行结构
	return IInputProcessor::HandleMouseButtonDownEvent(SlateApp, MouseEvent);
}

bool FPMSEdGraphPaneInputPreProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	// UpdateEventContext(SlateApp, MouseEvent);
	// if(CurContext.IsCursorInsidePanel)
	// {
	// 	return true;		
	// }
	// else
	// {
	// 	return false;
	// }
	return IInputProcessor::HandleMouseMoveEvent(SlateApp, MouseEvent);
}

bool FPMSEdGraphPaneInputPreProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	//FModifierKeysState State = StaticCast<FInputEvent>(InKeyEvent).ModifierKeys;
	//FModifierKeysState();
	
	bool CtrlState = InKeyEvent.IsControlDown();
	bool ShiftState = InKeyEvent.IsShiftDown();

	TArray<UPMSEdGraphNode*> SelectSourceNodes;
	if(CurContext.GraphPanel.IsValid())
	{
		for(UObject* Node : CurContext.GraphPanel->SelectionManager.GetSelectedNodes())
		{
			SelectSourceNodes.Add(Cast<UPMSEdGraphNode>(Node));
		}
		SameActionNodes = PMSEditorUtilities::GetNodesByConnectivity(SelectSourceNodes,	CtrlState, ShiftState);	
	}
	
	/*Todo 是否需要在这里加上这部分才可以让UICommandMap起作用？？
	if (IsCursorInsidePanel() && UICommandList->ProcessCommandBindings(InKeyEvent.GetKey(), FSlateApplication::Get().GetModifierKeys(), InKeyEvent.IsRepeat()))
	{
		return true;
	}
	*/
	
	if(InKeyEvent.GetCharacter() > 0 )
	{
		PressedCharKey = InKeyEvent.GetCharacter();
		auto Get = InKeyEvent.GetKey();
		if(Get == EKeys::RightAlt)
		{
			return true;
		}
	}
	if(InKeyEvent.GetKey() == EKeys::LeftShift)
	{
		return true;
	}
	return false;
	//PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,	bDownStream, bUpStream)
}

bool FPMSEdGraphPaneInputPreProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	return IInputProcessor::HandleKeyUpEvent(SlateApp, InKeyEvent);
}

bool FPMSEdGraphPaneInputPreProcessor::HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp,
	const FPointerEvent& MouseEvent)
{
	UpdateEventContext(SlateApp, MouseEvent);
	if(CurContext.IsCursorInsidePanel)
	{
		return true;		
	}
	return IInputProcessor::HandleMouseButtonDoubleClickEvent(SlateApp, MouseEvent);
}

TSharedPtr<SGraphPanel> FPMSEdGraphPaneInputPreProcessor::GetCurrentGraphPanel()
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

FVector2D FPMSEdGraphPaneInputPreProcessor::GraphPosToScreenPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D PanelPos)
{
	PanelPos = (PanelPos - GraphPanel->GetViewOffset()) * GraphPanel->GetZoomAmount();
	return Geometry.LocalToAbsolute(PanelPos);
}


FVector2D FPMSEdGraphPaneInputPreProcessor::ScreenPosToGraphPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D ScreenPos)
{
	auto ZoomStartOffset = Geometry.AbsoluteToLocal(ScreenPos);
	return GraphPanel->PanelCoordToGraphCoord(ZoomStartOffset);
}

void FPMSEdGraphPaneInputPreProcessor::OnSelectLinkedNodes(bool bDownStream, bool bUpStream)
{
	
	//PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,	bDownStream, bUpStream)
}
