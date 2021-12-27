#include "NodeDragHelper.h"
#include "../Editor/PMSEdGraphNode.h"
#include "../Editor/Utilities/PMSEditorUtilities.h"

void FNodeDragHelper::UpdateMoveTogetherNodes(UPMSEdGraphNode* EnterNode, TSet<UObject*> SelectedNodes, bool CtrlState,
	bool ShiftState)
{
	TArray<UPMSEdGraphNode*> SelectSourceNodes;
	if(SelectedNodes.Find(EnterNode))
	{		
		for(UObject* Node : SelectedNodes)
		{
			SelectSourceNodes.Add(Cast<UPMSEdGraphNode>(Node));
		}
	}
	else
	{
		SelectSourceNodes.Add(EnterNode);
	}
	TArray<UPMSEdGraphNode*> TempMoveTogetherNodes;
	TempMoveTogetherNodes = PMSEditorUtilities::GetNodesByConnectivity(SelectSourceNodes, CtrlState, ShiftState);
	TArray<UPMSEdGraphNode*> NewMoveTogetherNodes;
	TArray<FVector2D> NewMoveTogetherNodesStartPos;
	//NewMoveTogetherNodes.InsertZeroed(TempMoveTogetherNodes.Num());
	for(int i=0;i<MoveTogetherNodes.Num();i++)
	{
		UPMSEdGraphNode* PrvMoveTogetherNode =  MoveTogetherNodes[i];
		FVector2D CurNodeStartPos = MoveTogetherNodesStartPos[i];
		if(PrvMoveTogetherNode->StillMoveTogether)
		{
			NewMoveTogetherNodes.Add(PrvMoveTogetherNode);
			NewMoveTogetherNodesStartPos.Add(CurNodeStartPos);
		}
		else
		{
			PrvMoveTogetherNode->AlreadyMoveTogether = false;
			PrvMoveTogetherNode->StillMoveTogether = false;
			PrvMoveTogetherNode->NodePosX = CurNodeStartPos.X;
			PrvMoveTogetherNode->NodePosY = CurNodeStartPos.Y;
		}
	}
	for(UPMSEdGraphNode* TempMoveTogetherNode:TempMoveTogetherNodes)
	{
		if(!TempMoveTogetherNode->StillMoveTogether)
		{
			NewMoveTogetherNodes.Add(TempMoveTogetherNode);
			NewMoveTogetherNodesStartPos.Add(FVector2D(TempMoveTogetherNode->NodePosX,TempMoveTogetherNode->NodePosY));
		}
		TempMoveTogetherNode->StillMoveTogether = false;
	}
	MoveTogetherNodes.Reset();
	MoveTogetherNodesStartPos.Reset();
	MoveTogetherNodes = MoveTemp(NewMoveTogetherNodes);
	MoveTogetherNodesStartPos = MoveTemp(NewMoveTogetherNodesStartPos);
}
