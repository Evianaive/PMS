#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

class UPMSEdGraphNode;

struct FNodeDragHelper
{
	FVector2D DragNodeStartPos;
	TArray<UPMSEdGraphNode*> MoveTogetherNodes;
	TArray<FVector2D> MoveTogetherNodesStartPos;
	void UpdateMoveTogetherNodes(UPMSEdGraphNode* EnterNode, TSet<class UObject*> SelectedNodes, bool CtrlState, bool ShiftState);
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