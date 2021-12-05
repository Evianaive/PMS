#pragma once
#include "CoreMinimal.h"
#include "EditorViewportClient.h"

class FPMSEditor;
class UPMSEdGraph;
class FWorkflowCentricApplication;
class SPMSEditorViewport;

class PMSEDITOR_API FPMSEditorViewportClient : public FEditorViewportClient, public TSharedFromThis<FPMSEditorViewportClient>
{
public:
	FPMSEditorViewportClient(
		TWeakPtr<FPMSEditor> InPMSEditor, 
		TWeakPtr<SPMSEditorViewport> InPMSEditorViewport, 
		FPreviewScene& InPreviewScene,
		UPMSEdGraph* InGraph);

	virtual void Tick(float DeltaSeconds) override;
};