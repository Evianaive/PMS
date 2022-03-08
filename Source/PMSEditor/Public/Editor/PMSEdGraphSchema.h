// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphSchema.h"
#include "Editor/PMSEdGraphNode.h"
#include "PMSEdGraphSchema.generated.h"

/**
 * 
 */
USTRUCT()
struct PMSEDITOR_API FPMSEdGraphSchemaAction_NewNode :public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	// Simple type info
	static FName StaticGetTypeId() {static FName Type("FPMSEdGraphSchemaAction_NewNode"); return Type;}
	virtual FName GetTypeId() const override { return StaticGetTypeId(); } 
public:
	//要创建的UPMSEdGraphNode中为哪个UPMSGraphNode的子类
	UPROPERTY()
		class UClass* PMSGraphNodeClass;

	FPMSEdGraphSchemaAction_NewNode()
		: FEdGraphSchemaAction()
		, PMSGraphNodeClass(nullptr)
	{}

	FPMSEdGraphSchemaAction_NewNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, PMSGraphNodeClass(nullptr)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	static UPMSEdGraphNode* SpawnNode(UClass* PMSGraphNodeClass, UEdGraph* ParentGraph, UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode);
};

/*既要用来构建菜单的Node，也要用于点击时执行操作
 * 此处不必继承自FEdGraphSchemaAction
 */

struct PMSEDITOR_API FPMSEdGraphSchemaAction_ShelfTool : public FUIAction
{
	class UClass* PMSGraphNodeClass;
	FString IconName;
	FString Label;
	FString ToolName;
	UEdGraph* CurGraph;
	FVector2D SpawnPos;
	
	FPMSEdGraphSchemaAction_ShelfTool()
		: FUIAction()
		, PMSGraphNodeClass(nullptr)
		, IconName(L"polyexpand2d")
		, Label(L"Unknown")
		, ToolName(L"Unknown")
		, CurGraph(nullptr)
		, SpawnPos(FVector2D::ZeroVector)
	{
		Bind();
	}

	FPMSEdGraphSchemaAction_ShelfTool(UClass* InPMSGraphNodeClass,FString InToolName,FString InIconName,FString InLabel)
		: FUIAction()
		, PMSGraphNodeClass(InPMSGraphNodeClass)
		, IconName(InIconName)
		, Label(InLabel)
		, ToolName(InToolName)
		, CurGraph(nullptr)
		, SpawnPos(FVector2D::ZeroVector)
	{
		Bind();
	}
	
	void Bind()
	{
		ExecuteAction.BindRaw(this,&FPMSEdGraphSchemaAction_ShelfTool::BindedAction);
	}
	void BindedAction();
	// UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
	// static UPMSEdGraphNode* SpawnNode(UClass* PMSGraphNodeClass, UEdGraph* ParentGraph, UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode);
};


struct PMSEDITOR_API FPMSEdGraphSchemaAction_ShelfToolSubMenu
{	
	/*需要更改继承对象*/
	TMap<FName,FPMSEdGraphSchemaAction_ShelfToolSubMenu*> ChildrenSubMenu;
	TMap<FName,FPMSEdGraphSchemaAction_ShelfTool*> ChildrenToolAction;

	FPMSEdGraphSchemaAction_ShelfToolSubMenu()
	{}

	// virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

};

UCLASS()
class PMSEDITOR_API UPMSEdGraphSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()

	//void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	
	/*Right Click Menu*/
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const;
	void GetAllPMSNodeActions(FGraphContextMenuBuilder& ContexMenuBuilder) const;
	// void GetAllPMSShelfToolActions();
	
	/*Which Connection Between nodes are allowed*/
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;

	/*Define Drawing Policy*/
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID,
		float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,
		UEdGraph* InGraphObj) const override;

	// void GetAllPMSNodeActionsWithMenu() const;
	static void Init(bool bForce=false);
	FPMSEdGraphSchemaAction_ShelfToolSubMenu& GetPMSToolShelfLib() const
	{
		return PMSToolShelfLib;
	}
	
private:
	static void InitPMSGraphNodeClasses();
	static void InitPMSIconMapping();
	static void InitPMSToolShelfLib();
	static void RecursivelySort(FPMSEdGraphSchemaAction_ShelfToolSubMenu* SubMenu);

	static FPMSEdGraphSchemaAction_ShelfToolSubMenu PMSToolShelfLib;
	static TArray<UClass*> PMSGraphNodeClasses;
	static bool bPMSGraphNodeClassesInitialized;
	static TMap<FName,FName> PMSIconMapping;
};

// struct FPMSUIAction : public FUIAction
// {
// 	FString test = L"test";
// 	UEdGraph* Graph;
// 	FPMSUIAction()
// 	{
// 		ExecuteAction.BindSP(this,&FPMSUIAction::PeformAction);
// 	}
// 	void PeformAction()
// 	{
// 		
// 	}
// };