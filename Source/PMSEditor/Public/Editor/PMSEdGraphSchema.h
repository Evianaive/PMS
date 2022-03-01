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
	//Ҫ������UPMSEdGraphNode��Ϊ�ĸ�UPMSGraphNode������
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

/*��Ҫ���������˵���Node��ҲҪ���ڵ��ʱִ�в���
 * �˴����ؼ̳���FEdGraphSchemaAction
 */
USTRUCT()
struct PMSEDITOR_API FPMSEdGraphSchemaAction_ShelfTool : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY()

	// Simple type info
	static FName StaticGetTypeId() {static FName Type("FPMSEdGraphSchemaAction_ShelfTool"); return Type;}
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }
	
	UPROPERTY()
	class UClass* PMSGraphNodeClass;
	UPROPERTY()
	FString IconName;
	UPROPERTY()
	FString Label;
	
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
	GENERATED_USTRUCT_BODY()

	// Simple type info
	static FName StaticGetTypeId() {static FName Type("FPMSEdGraphSchemaAction_ShelfToolSubMenu"); return Type;}
	virtual FName GetTypeId() const override { return StaticGetTypeId(); }

	/*�˴������Ի��ɱ����������ܷ�����*/
	/*��Ҫ������������*/
	/*��Ҫ���ļ̳ж���*/
	TMap<FName,FEdGraphSchemaAction*> Children;

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
	static void Init(bool bForce=false);
	FPMSEdGraphSchemaAction_ShelfToolSubMenu* GetPMSToolShelfLib() const
	{
		return &PMSToolShelfLib;
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
