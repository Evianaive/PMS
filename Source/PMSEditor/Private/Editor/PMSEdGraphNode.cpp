// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphNode.h"

#include "GameplayDebuggerTypes.h"
#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

TSharedPtr<SGraphNode> UPMSEdGraphNode::CreateVisualWidget()
{
	SlateNode = SNew(SPMSEdGraphNode, this);
	return SlateNode;
}

void UPMSEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input0"));
	CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input1"));
	CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input2"));
	CreatePin(EGPD_Output,"PMSNodeInput",FName(),TEXT("Output0"));
	CreatePin(EGPD_Output,"PMSNodeInput",FName(),TEXT("Output1"));
	//CreatePin(EGPD_MAX,"PMSNodeInput",FName(),TEXT("In"));
}
