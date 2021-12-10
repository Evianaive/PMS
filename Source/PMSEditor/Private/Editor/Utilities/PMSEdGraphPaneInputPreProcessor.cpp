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
//#include "SGraphEditorImpl.h"
PMSEdGraphPaneInputPreProcessor::PMSEdGraphPaneInputPreProcessor()
{
	UICommandList->MapAction()
}

FPMSEventContex PMSEdGraphPaneInputPreProcessor::InitEventContex(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	FPMSEventContex ctx;
	ctx.IsDraggingConnection = SlateApp.GetDragDroppingContent()->IsOfType<FDragConnection>();

	FWidgetPath widgetsUnderCursor = SlateApp.LocateWindowUnderMouse(MouseEvent.GetScreenSpacePosition(), SlateApp.GetInteractiveTopLevelWindows());
	//Todo 为什么此处要构造这个？
	FScopedSwitchWorldHack SwitchWorld(widgetsUnderCursor);
	for(int i= widgetsUnderCursor.Widgets.Num()-1;i>=0;i--)
	{
		const auto curwidget = widgetsUnderCursor.Widgets[i];
		FString widgetName = curwidget.Widget->GetTypeAsString();
		if (widgetName == "SGraphPanel")
		{
			TSharedPtr<SGraphPanel> GraphPanel = StaticCastSharedRef<SGraphPanel>(widgetsUnderCursor.Widgets[i].Widget);
			if(ctx.GraphPanel->GetGraphObj()->GetClass()->GetFName().ToString() == "PMSEdGraph")
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

		if( widgetName.Contains("PMSEdGraphNode"))
		{
			ctx.GraphNode = StaticCastSharedRef<SPMSEdGraphNode>(curwidget.Widget);
			ctx.NodeGeometry = curwidget.Geometry;
		}
		if(widgetName.Contains("PMSEdGraphPin"))
		{
			//Todo SPMSEdGraphPin在编辑器中存在奇怪的位置偏移
			ctx.GraphPin = StaticCastSharedRef<SPMSEdGraphPin>(curwidget.Widget);
			ctx.PinGeometry = curwidget.Geometry;
		}
	}
	return ctx;
}

TSharedPtr<SGraphPanel> PMSEdGraphPaneInputPreProcessor::GetCurrentGraphPanel()
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

FVector2D PMSEdGraphPaneInputPreProcessor::GraphPosToScreenPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D PanelPos)
{
	PanelPos = (PanelPos - GraphPanel->GetViewOffset()) * GraphPanel->GetZoomAmount();
	return Geometry.LocalToAbsolute(PanelPos);
}


FVector2D PMSEdGraphPaneInputPreProcessor::ScreenPosToGraphPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D ScreenPos)
{
	auto ZoomStartOffset = Geometry.AbsoluteToLocal(ScreenPos);
	return GraphPanel->PanelCoordToGraphCoord(ZoomStartOffset);
}