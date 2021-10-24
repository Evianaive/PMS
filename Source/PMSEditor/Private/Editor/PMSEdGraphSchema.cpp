// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphSchema.h"
#include "UObject/UObjectIterator.h"
#include "PMSGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphNode.h"

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

	PMSEdGraphNodeToSpawn->NodePosX = Location.X;
	PMSEdGraphNodeToSpawn->NodePosY = Location.Y;
	PMSEdGraphNodeToSpawn->CreateNewGuid();
	PMSEdGraphNodeToSpawn->PostPasteNode();
	PMSEdGraphNodeToSpawn->AllocateDefaultPins();
	PMSEdGraphNodeToSpawn->IconName = Cast<UPMSGraphNode>(InPMSGraphNodeClass->GetDefaultObject())->IconName;

	ParentGraph->AddNode(PMSEdGraphNodeToSpawn);
	Cast<UPMSEdGraph>(ParentGraph)->SelectNode = PMSEdGraphNodeToSpawn;
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