// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"
#include "Modules/ModuleManager.h"

class FPMSEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FPMSEdGraphPanelInputPreProcessor> PMSInputPreProcessor;
};
