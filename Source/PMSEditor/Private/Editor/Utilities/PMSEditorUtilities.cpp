// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/Utilities/PMSEditorUtilities.h"


TArray<UPMSEdGraphNode*> PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,
	bool bDownStream, bool bUpStream)
{
	TArray<UPMSEdGraphNode*> NodesToReturn;
	//TArray<UPMSEdGraphNode*> NodesToCheck(SelectSourceNodes);
	//TArray<UPMSEdGraphNode*> NodesToCheck2(SelectSourceNodes);
	TArray<UPMSEdGraphNode*> NodesToCheck;
	for(UPMSEdGraphNode* SelectSourceNode :SelectSourceNodes)
	{
		SelectSourceNode->MoveTogetherChecked = true;
		SelectSourceNode->AlreadyMoveTogether = true;
		NodesToCheck.Add(SelectSourceNode);
	}
	
	while(NodesToCheck.Num()>0)
	{
		UPMSEdGraphNode* SourceNode = NodesToCheck.Pop();
		
		TArray<UEdGraphPin*> SourceNodePins = SourceNode->Pins;
		for(UEdGraphPin* SourceNodePin : SourceNodePins)
		{
			bool bOutandIn = true;
			if(bUpStream && SourceNodePin->Direction == EGPD_Input || bDownStream && SourceNodePin->Direction == EGPD_Output)
			{
				TArray<UEdGraphPin*> SourceNodePinLinks = SourceNodePin->LinkedTo;
				bOutandIn = (SourceNodePinLinks.Num()>0);
				for(UEdGraphPin* SourceNodePinLink : SourceNodePinLinks)
				{
					UPMSEdGraphNode* LinkedNode = Cast<UPMSEdGraphNode>(SourceNodePinLink->GetOwningNode());
					if(LinkedNode && !LinkedNode->MoveTogetherChecked)
					{
						NodesToReturn.Add(LinkedNode);
						NodesToCheck.Add(LinkedNode);
						LinkedNode->MoveTogetherChecked = true;
						LinkedNode->StillMoveTogether = LinkedNode->AlreadyMoveTogether;
						LinkedNode->AlreadyMoveTogether = true;
					}
				}	
			}
			if(!bOutandIn)
			{
				
			}
		}
	}
	for(UPMSEdGraphNode* ReturnNode : NodesToReturn)
	{
		ReturnNode->MoveTogetherChecked = false;
		UE_LOG(LogTemp, Log, TEXT("Node Name is %s"), *ReturnNode->NodeLabel.ToString());
	}
	for(UPMSEdGraphNode* SelectSourceNode :SelectSourceNodes)
	{
		SelectSourceNode->MoveTogetherChecked = false;
	}
	return NodesToReturn;
}
