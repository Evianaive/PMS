// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
// #include "GraphEditorModule.h"
#include "Editor/Utilities/PMSEdGraphPanelInputPreProcessor.h"
#include "Modules/ModuleManager.h"

class FPMSEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	TSharedPtr<FPMSEdGraphPanelInputPreProcessor> PMSInputPreProcessor;

private:
	friend class SGraphEditor_PMS;
	TSharedRef<SGraphEditor_PMS> PRIVATE_MakeGraphEditor_PMS(
		const TSharedPtr<FUICommandList>& InAdditionalCommands,
		const TAttribute<bool>& InIsEditable,
		const TAttribute<bool>& InDisplayAsReadOnly,
		const TAttribute<bool>& InIsEmpty,
		TAttribute<FGraphAppearanceInfo> Appearance,
		TSharedPtr<SWidget> InTitleBar,
		UEdGraph* InGraphToEdit,
		SGraphEditor::FGraphEditorEvents GraphEvents,
		bool InAutoExpandActionMenu,
		UEdGraph* InGraphToDiff,
		FSimpleDelegate InOnNavigateHistoryBack,
		FSimpleDelegate InOnNavigateHistoryForward,
		TAttribute<bool> ShowGraphStateOverlay);
};
