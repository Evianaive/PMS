// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/PMSGraphEditorWidgets/SGraphEditor_PMS.h"

#include "PMSEditorModule.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TArray< TWeakPtr<SGraphEditor_PMS> > SGraphEditor_PMS::AllInstances_PMS;

void SGraphEditor_PMS::Construct(const FArguments& InArgs)
{
	/* Parent Construct Content. In order to register the graph editor to GraphEditor::AllInstance
	EdGraphObj = InArgs._GraphToEdit;
	OnGraphModuleReloadedCallback = InArgs._OnGraphModuleReloaded;
	AssetEditorToolkit = InArgs._AssetEditorToolkit;

	// Register this widget with the module so that we can gracefully handle the module being unloaded.
	// See OnModuleUnloading()
	RegisterGraphEditor( SharedThis(this) );

	// Register a graph modified handler
	if (EdGraphObj != NULL)
	{
		EdGraphObj->AddOnGraphChangedHandler( FOnGraphChanged::FDelegate::CreateSP( this, &SGraphEditor::OnGraphChanged ) );
	}

	// Make the actual GraphEditor instance
	ConstructImplementation(InArgs);
	*/

	/* this->SGraphEditor::Construct(InArgs); */
	SGraphEditor::Construct(InArgs);
	ConstructImplementation_PMS(InArgs);
}

void SGraphEditor_PMS::RegisterGraphEditor_PMS(const TSharedRef<SGraphEditor_PMS>& InGraphEditor)
{
	// Compact the list of GraphEditor instances
	for (int32 WidgetIndex = 0; WidgetIndex < AllInstances_PMS.Num(); ++WidgetIndex)
	{
		if (!AllInstances_PMS[WidgetIndex].IsValid())
		{
			AllInstances_PMS.RemoveAt(WidgetIndex);
			--WidgetIndex;
		}
	}

	AllInstances_PMS.Add(InGraphEditor);
}

void SGraphEditor_PMS::ConstructImplementation_PMS(const FArguments& InArgs)
{
	FPMSEditorModule& GraphEdModule_PMS = FModuleManager::LoadModuleChecked<FPMSEditorModule>(TEXT("PMSEditor"));
	

	// Construct the implementation and make it the contents of this widget.
	Implementation_PMS = GraphEdModule_PMS.PRIVATE_MakeGraphEditor_PMS( InArgs._AdditionalCommands, 
		InArgs._IsEditable, 
		InArgs._DisplayAsReadOnly, 
		InArgs._IsEmpty,
		InArgs._Appearance,
		InArgs._TitleBar,
		InArgs._GraphToEdit,
		InArgs._GraphEvents,
		InArgs._AutoExpandActionMenu,
		InArgs._GraphToDiff,
		InArgs._OnNavigateHistoryBack,
		InArgs._OnNavigateHistoryForward,
		InArgs._ShowGraphStateOverlay
		);

	Implementation_PMS->AssetEditorToolkit = InArgs._AssetEditorToolkit;

	this->ChildSlot
	[
		SNew( SBox )
		.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GraphEditorPanel")))
		[
			Implementation_PMS.ToSharedRef()
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
