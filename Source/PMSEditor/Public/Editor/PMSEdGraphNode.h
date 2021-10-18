// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"

//#include "Slate.h"

#include "PMSEdGraphNode.generated.h"

class UEdGraphPin;

/**
 * 
 */
UCLASS()
class PMSEDITOR_API UPMSEdGraphNode : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	TSharedPtr<SGraphNode> CreateVisualWidget() override;
	TSharedPtr<SGraphNode>SlateNode;
	//TArray<UEdGraphPin> Pins;

	//Test adjust width
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		//FMargin Margin = FMargin(2.f, 2.f);
		FVector2D Margin = FVector2D(36.f, 12.f);
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		FString IconName = TEXT("polyexpand2d.svg");
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		FLinearColor Color = FLinearColor(0.1f, 0.1f, 0.1f);
};
