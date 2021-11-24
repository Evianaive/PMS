// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphSchema.h"
#include "UObject/UObjectIterator.h"
#include "PMSGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphNode.h"
#include "Editor/SlateWidgets/FPMSConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "PMSEdGraphSchema"

//Static Member of UPMSEdGraphSchema
TArray<UClass*> UPMSEdGraphSchema::PMSGraphNodeClasses;
bool UPMSEdGraphSchema::bPMSGraphNodeClassesInitialized = false;

UEdGraphNode* FPMSEdGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /* = true */) {
	
	return SpawnNode(PMSGraphNodeClass, ParentGraph, FromPin, Location, bSelectNewNode);
}

UPMSEdGraphNode* FPMSEdGraphSchemaAction_NewNode::SpawnNode(UClass* InPMSGraphNodeClass, UEdGraph* ParentGraph, UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode) {
	check(InPMSGraphNodeClass);
	UPMSEdGraphNode* PMSEdGraphNodeToSpawn = NewObject<UPMSEdGraphNode>(ParentGraph);
	const FScopedTransaction Transaction(LOCTEXT("ProceduralModelingSystemEditorNewPMSEdGraphNode", "PMSEditor: New PMSEdGraphNode"));
	ParentGraph->Modify();

	PMSEdGraphNodeToSpawn->NodePosX = Location.X-PMSEdGraphNodeToSpawn->NodeSize.X/2;
	PMSEdGraphNodeToSpawn->NodePosY = Location.Y-PMSEdGraphNodeToSpawn->NodeSize.Y/2;
	PMSEdGraphNodeToSpawn->CreateNewGuid();
	PMSEdGraphNodeToSpawn->PostPasteNode();
	PMSEdGraphNodeToSpawn->AllocateDefaultPins();
	PMSEdGraphNodeToSpawn->IconName = Cast<UPMSGraphNode>(InPMSGraphNodeClass->GetDefaultObject())->IconName;

	ParentGraph->AddNode(PMSEdGraphNodeToSpawn);
	if(!IsValid(Cast<UPMSEdGraph>(ParentGraph)->DisplayNode))
	{
		Cast<UPMSEdGraph>(ParentGraph)->DisplayNode = PMSEdGraphNodeToSpawn;
		PMSEdGraphNodeToSpawn->DisplayState = true;
	}
	//Cast<UPMSEdGraph>(ParentGraph)->SelectNode = PMSEdGraphNodeToSpawn;
	return PMSEdGraphNodeToSpawn;
}
UPMSEdGraphSchema::UPMSEdGraphSchema(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}
void UPMSEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const 
{

}

void UPMSEdGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	GetAllPMSNodeActions(ContextMenuBuilder);


}

const FPinConnectionResponse UPMSEdGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if(A->GetOwningNode()==B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,TEXT("You can't link Pins who belong to same Node"));
	}
	if(A->Direction==B->Direction)
	{
		FFormatNamedArguments InputType;
		if(A->Direction==EEdGraphPinDirection::EGPD_Input)
		{
			InputType.Add(TEXT("PinType"),FText::FromString("input"));
		}
		else
		{
			InputType.Add(TEXT("PinType"),FText::FromString("output"));
		}
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,FText::Format(LOCTEXT("NewConnectionFailTips","You can't link two {PinType}"),InputType));
	}
	//TODO 需要添加检测是否连线成环的部分
	return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B,TEXT("Create"));
}

FConnectionDrawingPolicy* UPMSEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FPMSConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,
	                                            InDrawElements, InGraphObj);
}

void UPMSEdGraphSchema::GetAllPMSNodeActions(FGraphContextMenuBuilder& ContexMenuBuilder) const
{
	InitPMSGraphNodeClasses();
	for (UClass* PMSGraphNodeClass : PMSGraphNodeClasses) {
		//it seems there is no need to add the next line.
		//if(PMSGraphNodeClass){
		UPMSGraphNode* PMSGraphNode = PMSGraphNodeClass->GetDefaultObject<UPMSGraphNode>();
		//UPMSGraphNode子类节点名字，用于后面显示提示用
		const FText NodeName = FText::FromString(PMSGraphNodeClass->GetName().RightChop(3));
		//可以用arguments的名称来格式化字符串，用于后面构建提示
		FFormatNamedArguments TooltipArgs;
		TooltipArgs.Add(TEXT("Name"), NodeName);
		
		const FText Tips = FText::Format(LOCTEXT("NewPMSGraphNodeActionTips", "Add {Name} node at here"), TooltipArgs);
		
		TSharedPtr<FPMSEdGraphSchemaAction_NewNode>NewPMSGraphNodeAction(new FPMSEdGraphSchemaAction_NewNode(FText::FromString("PMSNode"), NodeName, Tips, 0));
		NewPMSGraphNodeAction->PMSGraphNodeClass = PMSGraphNodeClass;
		ContexMenuBuilder.AddAction(NewPMSGraphNodeAction);
		
		//(new FPMSEdGraphSchemaAction_NewNode(FText::FromString("PMSNode"), NodeName, Tips, 0)).ToSharedPtr()
		//}
	}
}

void UPMSEdGraphSchema::InitPMSGraphNodeClasses()
{
	if (bPMSGraphNodeClassesInitialized) {
		return;
	}
	//if has been initialized, these codes will not run.
	//iter over all classes Resisted by UCLASS? And check if current class 
	//is a subclass of UPMSGraphNode and it is not a abstract class
	for (TObjectIterator<UClass> CurClass; CurClass; ++CurClass) {
		if (CurClass->IsChildOf(UPMSGraphNode::StaticClass()) && !CurClass->HasAnyClassFlags(CLASS_Abstract)) {
			PMSGraphNodeClasses.Add(*CurClass);
		}
	}
	PMSGraphNodeClasses.Sort();
	bPMSGraphNodeClassesInitialized = true;
}

#undef LOCTEXT_NAMESPACE