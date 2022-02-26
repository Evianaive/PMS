// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PMSEdGraphSchema.h"
#include "UObject/UObjectIterator.h"
#include "PMSGraphNode.h"
#include "PMSSubGraphNode.h"
#include "Editor/PMSEdGraph.h"
#include "Editor/PMSEdGraphNode.h"
#include "Editor/PMSEdSubGraphNode.h"
#include "Editor/SlateWidgets/PMSConnectionDrawingPolicy.h"
#include "Interfaces/IPluginManager.h"
#include "Utilities/tinyxml2.h"

#define LOCTEXT_NAMESPACE "PMSEdGraphSchema"

//Static Member of UPMSEdGraphSchema
TArray<UClass*> UPMSEdGraphSchema::PMSGraphNodeClasses;
FPMSEdGraphSchemaAction_ShelfToolSubMenu UPMSEdGraphSchema::PMSToolShelfLib;
bool UPMSEdGraphSchema::bPMSGraphNodeClassesInitialized = false;

UEdGraphNode* FPMSEdGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /* = true */) {
	
	return SpawnNode(PMSGraphNodeClass, ParentGraph, FromPin, Location, bSelectNewNode);
}

UPMSEdGraphNode* FPMSEdGraphSchemaAction_NewNode::SpawnNode(UClass* InPMSGraphNodeClass, UEdGraph* ParentGraph, UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode) {
	check(InPMSGraphNodeClass);
	// MakeUniqueObjectName()
	UPMSEdGraphNode* PMSEdGraphNodeToSpawn;
	if(InPMSGraphNodeClass->IsChildOf(UPMSSubGraphNode::StaticClass()))
	{
		PMSEdGraphNodeToSpawn = NewObject<UPMSEdSubGraphNode>(ParentGraph);
	}
	else
	{
		PMSEdGraphNodeToSpawn = NewObject<UPMSEdGraphNode>(ParentGraph);		
	}
	
	const FScopedTransaction Transaction(LOCTEXT("ProceduralModelingSystemEditorNewPMSEdGraphNode", "PMSEditor: New PMSEdGraphNode"));
	ParentGraph->Modify();

	PMSEdGraphNodeToSpawn->NodePosX = Location.X-PMSEdGraphNodeToSpawn->NodeSize.X/2;
	PMSEdGraphNodeToSpawn->NodePosY = Location.Y-PMSEdGraphNodeToSpawn->NodeSize.Y/2;
	PMSEdGraphNodeToSpawn->CreateNewGuid();
	PMSEdGraphNodeToSpawn->PostPasteNode();
	PMSEdGraphNodeToSpawn->AllocateDefaultPins();
	PMSEdGraphNodeToSpawn->IconName = Cast<UPMSGraphNode>(InPMSGraphNodeClass->GetDefaultObject())->IconName;
	//PMSEdGraphNodeToSpawn->GetNodeTitleColor()
	//NodeTile is the class name of node
	//PMSEdGraphNodeToSpawn->GetName();
	//PMSEdGraphNodeToSpawn->GetNodeTitle();
	PMSEdGraphNodeToSpawn->NodeLabel = FText::FromString(PMSEdGraphNodeToSpawn->IconName);
	ParentGraph->AddNode(PMSEdGraphNodeToSpawn);
	if(!IsValid(Cast<UPMSEdGraph>(ParentGraph)->DisplayNode))
	{
		Cast<UPMSEdGraph>(ParentGraph)->DisplayNode = PMSEdGraphNodeToSpawn;
		PMSEdGraphNodeToSpawn->DisplayState = true;
	}
	//Cast<UPMSEdGraph>(ParentGraph)->SelectNode = PMSEdGraphNodeToSpawn;
	return PMSEdGraphNodeToSpawn;
}

UEdGraphNode* FPMSEdGraphSchemaAction_ShelfTool::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode);
}

UPMSEdGraphNode* FPMSEdGraphSchemaAction_ShelfTool::SpawnNode(UClass* PMSGraphNodeClass, UEdGraph* ParentGraph,
	UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode)
{
	return nullptr;
}

UPMSEdGraphSchema::UPMSEdGraphSchema(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

UEdGraphNode* FPMSEdGraphSchemaAction_ShelfToolSubMenu::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin,
	const FVector2D Location, bool bSelectNewNode)
{
	return FEdGraphSchemaAction::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode);
}

void UPMSEdGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const 
{

}

void UPMSEdGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
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
	
	if(A->Direction==EEdGraphPinDirection::EGPD_Output)
	{
		// if(A->LinkedTo.Num() > 0)
		// {			
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B,TEXT("Create"));
		// }	
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A,TEXT("Create"));		
	}
	
	//TODO 需要添加检测是否连线成环的部分
}

FConnectionDrawingPolicy* UPMSEdGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
	float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
	UEdGraph* InGraphObj) const
{
	return new FPMSConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
	//return UEdGraphSchema::CreateConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void UPMSEdGraphSchema::GetAllPMSNodeActions(FGraphContextMenuBuilder& ContexMenuBuilder) const
{
	InitPMSGraphNodeClasses();
	InitPMSToolShelfLib();
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

void UPMSEdGraphSchema::InitPMSToolShelfLib()
{
	if (bPMSGraphNodeClassesInitialized) {
		return;
	}
	static FString ToolShelfsDir = IPluginManager::Get().FindPlugin(TEXT("PMS"))->GetBaseDir() / TEXT("Resources/ToolShelfs");
	TArray<FString> AllToolShelfFilesPath;
	IFileManager::Get().FindFilesRecursive(AllToolShelfFilesPath,*ToolShelfsDir,TEXT("*.shelf"),true,false);
	
	for (const auto ToolShelfFilePath : AllToolShelfFilesPath)
	{

		// if(ToolShelfFilePath!=FString(L"../../../../../../UnrealProject/ProceduralModeling/Plugins/PMS/Resources/ToolShelfs/SopToolsTest.shelf"))
		// 	continue;
		
		tinyxml2::XMLDocument ToolShelfFile;
		ToolShelfFile.LoadFile(TCHAR_TO_ANSI(*ToolShelfFilePath));
		auto RootNodeShelfDocument = ToolShelfFile.RootElement();
		auto NodeTool = RootNodeShelfDocument->FirstChildElement();
		while (NodeTool)
		{
			FString ToolName(NodeTool->Attribute("name"));
			FString ToolLabel(NodeTool->Attribute("label"));
			FString ToolIcon(NodeTool->Attribute("icon"));
			ToolIcon.RightChopInline(4);

			FString ToolPath = TEXT("Unkown");
			FString ToolScriptType = TEXT("python");
			FString ToolScript = TEXT("No Script");
			
			if(tinyxml2::XMLElement* NodeToolPath = NodeTool->FirstChildElement("toolSubmenu"))
			{
				ToolPath = FString(NodeToolPath->GetText());
			}
			if(tinyxml2::XMLElement* NodeToolScript = NodeTool->FirstChildElement("script"))
			{
				ToolScriptType = NodeToolScript->FirstAttribute()->Value();
				auto FC = NodeToolScript->FirstChild();
				ToolScript = FString(FC->Value());
			}
			TArray<FString> ToolPathArray;
			ToolPath.ParseIntoArray(ToolPathArray,TEXT("/"),true);

			/*Add Tool*/
			FPMSEdGraphSchemaAction_ShelfToolSubMenu* CurSubMenu = &PMSToolShelfLib;
			for(auto CurToolPath:ToolPathArray)
			{
				FEdGraphSchemaAction* NextSubMenu = CurSubMenu->Children.FindOrAdd(FName(CurToolPath),new FPMSEdGraphSchemaAction_ShelfToolSubMenu());
				CurSubMenu = (FPMSEdGraphSchemaAction_ShelfToolSubMenu*)(NextSubMenu);
			}
			auto InClass = UPMSGraphNode::StaticClass();
			CurSubMenu->Children.FindOrAdd(FName(ToolName),new FPMSEdGraphSchemaAction_ShelfTool(InClass,ToolIcon,ToolLabel));
			
			NodeTool = NodeTool->NextSiblingElement();
		}		
	}
	/*需要实现Sort*/
	// PMSToolShelfLib.Children.KeyStableSort([](FName A,FName B){return !!A.Compare(B);});
	
}

#undef LOCTEXT_NAMESPACE
