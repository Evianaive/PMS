// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/Utilities/PMSEditorUtilities.h"


TArray<UPMSEdGraphNode*> PMSEditorUtilities::GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes,
	bool bDownStream, bool bUpStream)
{
	TArray<UPMSEdGraphNode*> NodesToReturn;
	TArray<UPMSEdGraphNode*> NodesToCheck = SelectSourceNodes;
	for(UPMSEdGraphNode* SelectSourceNode :SelectSourceNodes)
	{
		SelectSourceNode->MoveTogether = 1;
	}
	
	while(NodesToCheck.Num()>0)
	{
		UPMSEdGraphNode* SourceNode = NodesToCheck.Pop();
		
		TArray<UEdGraphPin*> SourceNodePins = SourceNode->Pins;
		for(UEdGraphPin* SourceNodePin : SourceNodePins)
		{
			if(bUpStream && SourceNodePin->Direction == EGPD_Input || bDownStream && SourceNodePin->Direction == EGPD_Output)
			{
				TArray<UEdGraphPin*> SourceNodePinLinks = SourceNodePin->LinkedTo;
				for(UEdGraphPin* SourceNodePinLink : SourceNodePinLinks)
				{
					UPMSEdGraphNode* LinkedNode = Cast<UPMSEdGraphNode>(SourceNodePinLink->GetOwningNode());
					if(LinkedNode && !LinkedNode->MoveTogether)
					{
						NodesToReturn.Add(LinkedNode);
						NodesToCheck.Add(LinkedNode);
						LinkedNode->MoveTogether = true;
					}
				}	
			}
		}
	}
	return NodesToReturn;
}
