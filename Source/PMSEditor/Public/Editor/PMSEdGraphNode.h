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
	void PMSSnapToGrid(float GridSnapSize, float Snapthreshold);
	
	//Test adjust width
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		//FMargin Margin = FMargin(2.f, 2.f);
		FVector2D Margin = FVector2D(48.f, 48.f);
	UPROPERTY(Category="NodeSetting",EditAnywhere)
		FVector2D NodePadding = FVector2D(16.0f,6.0f);
	UPROPERTY(Category="NodeSetting",EditAnywhere)
		FVector2D NodeSize = FVector2D(200.0f,60.0f);
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		FString IconName = TEXT("polyexpand2d.svg");
	UPROPERTY(Category = "NodeSetting", EditAnywhere)
		FLinearColor Color = FLinearColor(0.7f, 0.7f, 0.7f);
	//����������һ���Զ���Ľṹ�岢����UPROPERTY�����?�ṹ���ǲ���UStruct�᲻�����ܷ�ɹ������йأ�
	//��Ҫ�Զ���ṹ�����ʾ���
	//https://easycomplex-tech.com/cnblog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorCustomProp/
};
