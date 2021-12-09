// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct FPMSNodeNameEntryId
{
	FPMSNodeNameEntryId() : Value(0) {}
	FPMSNodeNameEntryId(ENoInit) {}
private:
	uint32 Value;
};
class PMSEDITOR_API FPMSNodeName
{
public:
	FPMSNodeName();
	~FPMSNodeName();

private:
	FPMSNodeNameEntryId ComparisonIndex;
	uint32 Number;
};
