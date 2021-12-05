#include "Editor/Viewport/PMSViewportClient.h"
#include "Editor/Viewport/SPMSViewportClient.h"
#include "AssetEditorModeManager.h"


FPMSEditorViewportClient::FPMSEditorViewportClient(
	TWeakPtr<FPMSEditor> InPMSEditor, 
	TWeakPtr<SPMSEditorViewport> InPMSEditorViewport, 
	FPreviewScene& InPreviewScene,
	UPMSEdGraph* InGraph)
	:FEditorViewportClient(nullptr, &InPreviewScene)
{
	//((FAssetEditorModeManager*)ModeTools)->SetPreviewScene(PreviewScene);

	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = true;
	DrawHelper.GridColorAxis = FColor(70, 70, 70);
	DrawHelper.GridColorMajor = FColor(40, 40, 40);
	DrawHelper.GridColorMinor = FColor(20, 20, 20);
	DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;

	DrawHelper.bDrawPivot = true;

	SetRealtime(true);

	SetViewMode(VMI_Lit);
	OverrideNearClipPlane(1.0f);
	SetViewLocation(FVector(500, 300, 500));
}
void FPMSEditorViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
}

