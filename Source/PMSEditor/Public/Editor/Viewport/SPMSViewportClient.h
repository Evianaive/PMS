#pragma once

#include "SEditorViewport.h"
//#include "AssetArchitectToolkit.h"
#include "CoreMinimal.h"
#include "Editor/PMSEdGraph.h"
//#include "PMSEditor.h"

class FPMSEditor;
class PMSEDITOR_API SPMSEditorViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SPMSEditorViewport) {}
		SLATE_ARGUMENT(TWeakPtr<FPMSEditor>, PMSEditorPtr)
		SLATE_ARGUMENT(UPMSEdGraph*, GraphToView)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
	TWeakPtr<FPMSEditor> PMSEditorPtr;
	UPMSEdGraph* GraphToView;

	TSharedPtr<class FAdvancedPreviewScene> PreviewScene;

	class USkyLightComponent* Skylight;
	class UAtmosphericFogComponent* AtmosphericFog;

	TSharedPtr<class FPMSEditorViewportClient> EditorViewportClient;
};
