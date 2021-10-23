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

UCLASS()
class PMSEDITOR_API UPMSEdGraphSchema : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()
	
public:
	//void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const;

	void GetAllPMSNodeActions(FGraphContextMenuBuilder& ContexMenuBuilder) const;


private:
	static void InitPMSGraphNodeClasses();

	static TArray<UClass*> PMSGraphNodeClasses;

	static bool bPMSGraphNodeClassesInitialized;
};
