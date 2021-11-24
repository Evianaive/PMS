// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/Input/SCheckBox.h"

/**
 * 
 */
class SNodeFlagCheckBox : public SCheckBox
{
public:
 virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
};
