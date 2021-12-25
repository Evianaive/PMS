// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphPanel.h"
#include "CoreMinimal.h"
/**
 * 
 */
class PMSEDITOR_API SPMSGraphPanel : public SGraphPanel
{
public:
	SLATE_BEGIN_ARGS(SPMSGraphPanel)
	{}
	SLATE_END_ARGS()
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
