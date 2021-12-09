// // Fill out your copyright notice in the Description page of Project Settings.
// #pragma once
// #include "Editor/Utilities/PMSPanelFactory.h"
//
// #include "../../../Public/Editor/PMSEdGraphNode.h"
// #include "../../../Public/Editor/SlateWidgets/SPMSEdGraphNode.h"
// #include "../../../Public/Editor/SlateWidgets/SPMSEdGraphPin.h"
// //#include "Editor/PMSEdGraphNode.h"
// //#include "Editor/SlateWidgets/SPMSEdGraphNode.h"
//
// TSharedPtr<SGraphNode> FPMSNodeFactory::CreateNode(UEdGraphNode* Node) const
// {
// 	if(UPMSEdGraphNode* NewNode = Cast<UPMSEdGraphNode>(Node))
// 	{
// 		return SNew(SPMSEdGraphNode,NewNode);
// 	}
// 	return NULL;
// }
//
// TSharedPtr<SGraphPin> FPMSPinFactory::CreatePin(UEdGraphPin* Pin) const
// {
// 	if(UPMSEdGraphNode* PinNode = Cast<UPMSEdGraphNode>(Pin->GetOuter()))
// 	{
// 		return SNew(SPMSEdGraphPin, Pin);
// 	}
// 	return NULL;
// }
//
// class FConnectionDrawingPolicy* FPMSPinConnectionFactory::CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
// {
// 	if(Schema->IsA(UPMSEdGraphSchema::StaticClass()))
// 	{
// 		return new FPMS
// 	}
// }