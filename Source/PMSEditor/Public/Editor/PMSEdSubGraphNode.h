// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PMSEdGraphNode.h"
#include "UObject/Object.h"
#include "PMSEdSubGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class PMSEDITOR_API UPMSEdSubGraphNode : public UPMSEdGraphNode
{
	GENERATED_BODY()
public:
	TObjectPtr<UPMSEdGraph> SubGraph;

};
