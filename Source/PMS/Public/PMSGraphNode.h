// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PMSGraphNode.generated.h"

/**
 * 
 */
UCLASS(abstract)
class PMS_API UPMSGraphNode : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	FString IconName;
};


#define PMSNODE_BGEIN_ARGS(NodeName) \
	class PMS_API U##NodeName## : public UPMSGraphNode \
	{ \
		GENERATED_BODY() \
	public:\
		U##NodeName##();
#define PMSNODE_END_ARGS() \
	};