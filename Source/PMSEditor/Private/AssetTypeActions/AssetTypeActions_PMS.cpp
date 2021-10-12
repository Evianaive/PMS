#include "AssetTypeActions/AssetTypeActions_PMS.h"

#include <Modules/ModuleManager.h>
#include "PMSEditorModule.h"
#include "PMSGraph.h"
#include "Editor/PMSEditor.h"

FAssetTypeActions_PMS::FAssetTypeActions_PMS()
{
}
FText FAssetTypeActions_PMS::GetName() const 
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_PMS", "Procedural Modeling System");
}

FColor FAssetTypeActions_PMS::GetTypeColor() const 
{
	// FAppStyle::Get().GetSlateColor("Colors.Primary")
	return FColor(139, 119, 101);
}

UClass* FAssetTypeActions_PMS::GetSupportedClass() const 
{
	return UPMS_Graph::StaticClass();
}

void FAssetTypeActions_PMS::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) 
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Object = InObjects.CreateConstIterator(); Object; Object++)
	{
		auto Graph = Cast<UPMS_Graph>(*Object);
		if (Graph != nullptr)
		{
			TSharedRef<FPMSEditor>EditorToolkit = MakeShareable(new FPMSEditor());
			EditorToolkit->InitPMSAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_PMS::GetCategories() 
{
	// why this is returned?
	return EAssetTypeCategories::Gameplay;
}
