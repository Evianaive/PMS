// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraph.h"

#include "Editor/PMSEdGraphNode.h"

void UPMSEdGraph::AddTestNode()
{
	UPMSEdGraphNode* TestNode = NewObject<UPMSEdGraphNode>(this);
	this->Modify();
	TestNode->SetFlags(RF_Transactional);
	this->AddNode(TestNode, true);
	TestNode->CreateNewGuid();
	TestNode->NodePosX = 0;
	TestNode->NodePosY = 0;
}
