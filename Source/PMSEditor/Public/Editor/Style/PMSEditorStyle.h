// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/ISlateStyle.h"

/**
 * 
 */
class PMSEDITOR_API FPMSEditorStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for niagara editor widgets */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();

private:

	static TSharedRef< class FSlateStyleSet > Create();

private:

	static TSharedPtr< class FSlateStyleSet > PMSEditorStyleInstance;
};
