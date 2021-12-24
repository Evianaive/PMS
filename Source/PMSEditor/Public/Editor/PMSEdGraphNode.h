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
	/*
	 *store a Ptr of PMSGraphNode and use the PMSGraphNode in AllocateDefaultPins()
	 */
	TSharedPtr<SGraphNode>SlateNode;
	//TArray<UEdGraphPin> Pins;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual void AllocateDefaultPins() override;	
	void PMSSnapToGrid(float GridSnapSize, float Snapthreshold, TArray<FVector2D> PossibleSnapPos);
	
	//Test adjust width
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		//FMargin Margin = FMargin(2.f, 2.f);
	FVector2D Margin = FVector2D(48.f, 48.f);
	UPROPERTY(Category="NodeSetting",EditAnywhere)
	FVector2D NodePadding = FVector2D(15.0f,5.0f);
	UPROPERTY(Category="NodeSetting",EditAnywhere)
	FVector2D NodeSize = FVector2D(200.0f,60.0f);
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
	FString IconName = TEXT("polyexpand2d.svg");
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
	FLinearColor Color = FLinearColor(0.7f, 0.7f, 0.7f);

	UPROPERTY(Category = "NodeSetting", EditAnywhere)
	FText NodeLabel = FText::FromString("null");
	
	UPROPERTY(Category = "NodeState", EditAnywhere)
	bool DisplayState = false;
	UPROPERTY(Category = "NodeState", EditAnywhere)
	bool TemplateState = false;
	UPROPERTY(Category = "NodeState", EditAnywhere)
	bool LockState = false;
	UPROPERTY(Category = "NodeState", EditAnywhere)
	bool BypassState = false;

	/*For User Interface*/
	bool MoveTogetherChecked = false;
	//UPROPERTY(Category = "MoveState", EditAnywhere)
	bool AlreadyMoveTogether = false;
	//UPROPERTY(Category = "MoveState", EditAnywhere)
	bool StillMoveTogether = false;
	//如果在这里加一个自定义的结构体并挂上UPROPERTY会如何?结构体是不是UStruct会不会与能否成功编译有关？
	//需要自定义结构体的显示面板
	//https://easycomplex-tech.com/cnblog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorCustomProp/
};
