// Fill out your copyright notice in the Description page of Project Settings.

#include "Editor/Utilities/PMSGraphPaneInputPreProcessor.h"
//#include "SlateApplication.h"
#include "SGraphPanel.h"
//#include "SGraphEditorImpl.h"

TSharedPtr<SGraphPanel> PMSGraphPaneInputPreProcessor::GetCurrentGraphPanel()
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

FVector2D PMSGraphPaneInputPreProcessor::GraphPosToScreenPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D PanelPos)
{
	PanelPos = (PanelPos - GraphPanel->GetViewOffset()) * GraphPanel->GetZoomAmount();
	return Geometry.LocalToAbsolute(PanelPos);
}


FVector2D PMSGraphPaneInputPreProcessor::ScreenPosToGraphPos(TSharedRef<SGraphPanel> GraphPanel, FGeometry Geometry, FVector2D ScreenPos)
{
	auto ZoomStartOffset = Geometry.AbsoluteToLocal(ScreenPos);
	return GraphPanel->PanelCoordToGraphCoord(ZoomStartOffset);
}