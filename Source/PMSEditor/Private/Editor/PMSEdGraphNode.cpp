// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphNode.h"

#include "GameplayDebuggerTypes.h"
#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

TSharedPtr<SGraphNode> UPMSEdGraphNode::CreateVisualWidget()
{
	SlateNode = SNew(SPMSEdGraphNode, this);
	// SlateNode->OnMouseButtonDoubleClick()
	//SlateNode = MakeTDecl<SPMSEdGraphNode>( "SPMSEdGraphNode", __FILE__, __LINE__, RequiredArgs::MakeRequiredArgs(this, Display) ) <<=  SPMSEdGraphNode::FArguments();
	return SlateNode;
}

void UPMSEdGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input0"));
	//CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input1"));
	//CreatePin(EGPD_Input,"PMSNodeInput",FName(),TEXT("Input2"));
	CreatePin(EGPD_Output,"PMSNodeInput",FName(),TEXT("Output0"));
	//CreatePin(EGPD_Output,"PMSNodeInput",FName(),TEXT("Output1"));
	//CreatePin(EGPD_MAX,"PMSNodeInput",FName(),TEXT("In"));
}


void UPMSEdGraphNode::PMSSnapToGrid(float GridSnapSize, float Snapthreshold, TArray<FVector2D> PossibleSnapPosArray)
{
	int32 NewNodePosX = GridSnapSize * FMath::RoundToInt((NodePosX + NodeSize.X/2.f)/GridSnapSize) - NodeSize.X/2.f;
	int32 NewNodePosY = GridSnapSize * FMath::RoundToInt((NodePosY + NodeSize.Y/2.f)/GridSnapSize) - NodeSize.Y/2.f;
	for(FVector2D PossibleSnapPos : PossibleSnapPosArray)
    {
        NewNodePosX = FMath::Abs(PossibleSnapPos.X - NodePosX)<FMath::Abs(NewNodePosX-NodePosX)? PossibleSnapPos.X : NewNodePosX;
		NewNodePosY = FMath::Abs(PossibleSnapPos.Y - NodePosY)<FMath::Abs(NewNodePosY-NodePosY)? PossibleSnapPos.Y : NewNodePosY;
    }
	if(FMath::Abs(NewNodePosX-NodePosX)<Snapthreshold)
	{
		NodePosX = NewNodePosX;
	}
	if(FMath::Abs(NewNodePosY-NodePosY)<Snapthreshold)
	{
		NodePosY = NewNodePosY;		
	}
	
}

void UPMSEdGraphNode::AddPin()
{
	CreatePin(EGPD_Output,"PMSNodeOutput",FName(),TEXT("Output1"));
}
