// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "EdGraphUtilities.h"
//
// /**
//  * 
//  */
// class PMSEDITOR_API FPMSNodeFactory : public  FGraphPanelNodeFactory
// {
// public:
// 	virtual TSharedPtr<class SGraphNode> CreateNode(class UEdGraphNode* Node) const override;
// };
//
// class PMSEDITOR_API FPMSPinFactory : public FGraphPanelPinFactory
// {
// public:
// 	virtual TSharedPtr<class SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
// };
//
// class PMSEDITOR_API FPMSPinConnectionFactory : public FGraphPanelPinConnectionFactory
// {
// public:
// 	virtual class FConnectionDrawingPolicy* CreateConnectionPolicy(const class UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const class FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
// };