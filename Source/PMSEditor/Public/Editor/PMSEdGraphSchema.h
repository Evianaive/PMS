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
	GENERATED_BODY()
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

/*既要用来构建菜单的Node，也要用于点击时执行操作*/
USTRUCT()
struct PMSEDITOR_API FPMSEdGraphSchemaAction_ShelfTool : public FEdGraphSchemaAction
{
	UPROPERTY()
	class UClass* PMSGraphNodeClass;
	UPROPERTY()
	FString IconName;
	UPROPERTY()
	FString Label;
	
	GENERATED_BODY()
	FPMSEdGraphSchemaAction_ShelfTool()
		: FEdGraphSchemaAction()
		, PMSGraphNodeClass(nullptr)
		, IconName(L"polyexpand2d")
		, Label(L"Unknown")
	{}

	FPMSEdGraphSchemaAction_ShelfTool(UClass* InPMSGraphNodeClass,FString InIconName,FString InLabel)
		: FEdGraphSchemaAction()
		, PMSGraphNodeClass(InPMSGraphNodeClass)
		, IconName(InIconName)
		, Label(InLabel)
	{}

	FPMSEdGraphSchemaAction_ShelfTool(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, PMSGraphNodeClass(nullptr)	
		, IconName("polyexpand2d")
		, Label(L"Unknown")
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

	static UPMSEdGraphNode* SpawnNode(UClass* PMSGraphNodeClass, UEdGraph* ParentGraph, UEdGraphPin* FromPin, FVector2D Location, bool bSelectNewNode);
};

USTRUCT()
struct PMSEDITOR_API FPMSEdGraphSchemaAction_ShelfToolSubMenu : public FEdGraphSchemaAction
{
	TMap<FName,FEdGraphSchemaAction*> Children;
	GENERATED_BODY()
	FPMSEdGraphSchemaAction_ShelfToolSubMenu()
		: FEdGraphSchemaAction()
	{}

	FPMSEdGraphSchemaAction_ShelfToolSubMenu(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		: FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

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
private:
	static void InitPMSGraphNodeClasses();
	static void InitPMSToolShelfLib();

private:
	static TArray<UClass*> PMSGraphNodeClasses;
	static FPMSEdGraphSchemaAction_ShelfToolSubMenu PMSToolShelfLib;
	static bool bPMSGraphNodeClassesInitialized;
};
