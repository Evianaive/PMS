#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

class UPMSEdGraphNode;

struct FNodeDragHelper
{
	FVector2D DragNodeStartPos;
	TArray<UPMSEdGraphNode*> MoveTogetherNodes;
	TArray<FVector2D> MoveTogetherNodesStartPos;
	bool bCtrlState;
	bool bShiftState;
	bool bAltState;
	void UpdateMoveTogetherNodes(UPMSEdGraphNode* EnterNode, TSet<class UObject*> SelectedNodes, bool CtrlState, bool ShiftState);
};

struct FInteractionPayLoad
{
	// Track Mouse movement after press left button on node or space
	// block zoom level change if cutting
	bool bCutKeyState = false;
	TArray<FVector2D> CursorTraceAfterDown = TArray<FVector2D>();

	// FVector2D CurPos;
	// void AppendPointByCursorDelta(FVector2D CursorDelta)
	// {
	// 	FVector2D& LastPoint = CursorTraceAfterDown[CursorTraceAfterDown.Num()-1];
	// 	CursorTraceAfterDown.Add(LastPoint + CursorDelta);
	// }
};

UENUM()
enum class EContextEnterState : uint8
{
	/* Mouse down on Node */
	OnNode,
	/* Mouse down on space */
	OnSpace,
	/* Mouse down on line */
	OnLine,
	/* Mouse down on pin */
	OnPin,
	/* Mouse down on node flag */
	OnFlag,
	/* None When Init*/
	None
};

UENUM()
enum class EMouseEnterState : uint8
{
	Left,
	Middle,
	Right,
	None
};


#define UpdateMoveTogetherNodesPos(NodeDragHelper,UNodeBeingDrag)\
for(int i=0;i<NodeDragHelper.MoveTogetherNodes.Num();i++)\
{\
	UPMSEdGraphNode* MoveTogetherNode = NodeDragHelper.MoveTogetherNodes[i];\
	FVector2D CurNodeStartPos = NodeDragHelper.MoveTogetherNodesStartPos[i];\
	MoveTogetherNode->NodePosX = CurNodeStartPos.X + UNodeBeingDrag->NodePosX - NodeDragHelper.DragNodeStartPos.X;\
	MoveTogetherNode->NodePosY = CurNodeStartPos.Y + UNodeBeingDrag->NodePosY - NodeDragHelper.DragNodeStartPos.Y;\
}