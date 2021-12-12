// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/PMSEdGraphNode.h"

/**
 * 
 */
class PMSEDITOR_API PMSEditorUtilities
{
public:
	PMSEditorUtilities(){};
	static TArray<UPMSEdGraphNode*> GetNodesByConnectivity(TArray<UPMSEdGraphNode*> SelectSourceNodes, bool bDownStream, bool bUpStream);  
};
