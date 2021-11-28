// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <EdGraph/EdGraph.h>
#include "Editor/PMSEdGraphNode.h"
#include "PMSEdGraph.generated.h"

/**
 * 
 */
UCLASS()
class PMSEDITOR_API UPMSEdGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UEdGraphNode> DisplayNode;
	
	UPROPERTY()
	TSet<TObjectPtr<UEdGraphNode>> TemplateNodes;
	
};
