// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphNode.h"

#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

TSharedPtr<SGraphNode> UPMSEdGraphNode::CreateVisualWidget()
{
	SlateNode = SNew(SPMSEdGraphNode, this);
	return SlateNode;
}