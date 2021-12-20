// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

/**
 * 
 */
class PMSEDITOR_API SPMSEdGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SPMSEdGraphPin)
		: _PinLabelStyle(NAME_DefaultPinLabelStyle)
		, _UsePinColorForText(false)
		, _SideToSideMargin(0.0f)
		{}
		SLATE_ARGUMENT(FName, PinLabelStyle)
		SLATE_ARGUMENT(bool, UsePinColorForText)
		SLATE_ARGUMENT(float, SideToSideMargin)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs,UEdGraphPin* InPin);
	virtual FSlateColor GetPinColor() const override;
protected:
	virtual const FSlateBrush* GetPinIcon() const override;
	TOptional<EMouseCursor::Type> GetPMSPinCursor() const;
};
