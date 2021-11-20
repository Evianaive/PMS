// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editor/Style/PMSEditorStyle.h"

#include "EditorStyleSet.h"
#include "Styling/SlateTypes.h"
#include "Styling/CoreStyle.h"
#include "Classes/EditorStyleSettings.h"
#include "Styling/StarshipCoreStyle.h"
#include "Styling/StyleColors.h"

#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FPMSEditorStyle::PMSEditorStyleInstance = NULL;

void FPMSEditorStyle::Initialize()
{
	if (!PMSEditorStyleInstance.IsValid())
	{
		PMSEditorStyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*PMSEditorStyleInstance);
	}
}

void FPMSEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*PMSEditorStyleInstance);
	ensure(PMSEditorStyleInstance.IsUnique());
	PMSEditorStyleInstance.Reset();
}

FName FPMSEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("PMSEditorStyle"));
	return StyleSetName;
}

PMSEDITOR_API FString RelativePathToPluginPath(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(TEXT("PMS"))->GetBaseDir() / TEXT("Resources");
	return (ContentDir / RelativePath) + Extension;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define IMAGE_CORE_BRUSH( RelativePath, ... ) FSlateImageBrush( FPaths::EngineContentDir() / "Editor/Slate" / RelativePath + TEXT(".png"), __VA_ARGS__ )
#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( RelativePathToPluginPath( RelativePath, ".png" ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_CORE_BRUSH( RelativePath, ... ) FSlateBoxBrush( FPaths::EngineContentDir() / "Editor/Slate" / RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BOX_PLUGIN_BRUSH( RelativePath, ... ) FSlateBoxBrush( RelativePathToPluginPath( RelativePath, ".png"), __VA_ARGS__ )
#define BORDER_CORE_BRUSH( RelativePath, ... ) FSlateBorderBrush( FPaths::EngineContentDir() / "Editor/Slate" / RelativePath + TEXT(".png"), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define DEFAULT_FONT(...) FStarshipCoreStyle::GetDefaultFontStyle(__VA_ARGS__)
#define IMAGE_PLUGIN_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( RelativePathToPluginPath( RelativePath, ".svg" ), __VA_ARGS__ )

#define IMAGE_BRUSH_SVG( AbsolutePath, ... ) FSlateVectorImageBrush(AbsolutePath, __VA_ARGS__)

const FVector2D Icon8x8(8.0f, 8.0f);
const FVector2D Icon12x12(12.0f, 12.0f);
const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

namespace PMSEditorStyleImpl {

void InitStats(TSharedRef< FSlateStyleSet > Style)
{
	// const FTextBlockStyle CategoryText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("DetailsView.CategoryTextStyle");
	// const FSlateColor SelectionColor = FEditorStyle::GetSlateColor("SelectionColor");
	// const FSlateColor SelectionColor_Pressed = FEditorStyle::GetSlateColor("SelectionColor_Pressed");
	//
	// Style->Set("PMSEditor.StatsText", CategoryText);
}

void InitNodeIcons(TSharedRef< FSlateStyleSet > Style)
{
	// Icons
	// Style->Set("PMSEditor.Isolate", new IMAGE_PLUGIN_BRUSH("Icons/Isolate", Icon16x16));
	// Style->Set("PMSEditor.Module.Pin.TypeSelector", new IMAGE_PLUGIN_BRUSH("Icons/Scratch", Icon16x16, FLinearColor::Gray));
	// Style->Set("PMSEditor.Module.AddPin", new IMAGE_PLUGIN_BRUSH("Icons/PlusSymbol_12x", Icon12x12, FLinearColor::Gray));
	// Style->Set("PMSEditor.Module.RemovePin", new IMAGE_PLUGIN_BRUSH("Icons/MinusSymbol_12x", Icon12x12, FLinearColor::Gray));
	// Style->Set("PMSEditor.Scratch", new IMAGE_PLUGIN_BRUSH("Icons/Scratch", Icon16x16, FLinearColor::Yellow));
	// Style->Set("PMSEditor.Message.CustomNote", new IMAGE_PLUGIN_BRUSH("Icons/icon_custom_note_16x", Icon16x16));

	TArray<FString> SvgIcons;
	IFileManager::Get().FindFilesRecursive(SvgIcons,*(Style->GetContentRootDir()/TEXT("Icons")),TEXT("*.svg"),true,false);
	for (FString SvgIcon : SvgIcons)
	{
		FString StyleName = FString("PMSEditor.NodeIcons.") + FPaths::GetBaseFilename(SvgIcon);
		Style->Set(FName(StyleName),new IMAGE_BRUSH_SVG(SvgIcon,Icon64x64));
	}
	// Style->Set("PMSEditor.NodeIcons.add",new IMAGE_BRUSH_SVG(SvgIcons[0],Icon64x64));
	// Style->Set("PMSEditor.NodeIcons.agentcollisionlayer",new IMAGE_BRUSH_SVG(SvgIcons[1],Icon64x64));
}

// void InitAssetColors(TSharedRef< FSlateStyleSet > Style)
// {
// 	// Asset colors
// 	Style->Set("PMSEditor.AssetColors.System", FLinearColor(1.0f, 0.0f, 0.0f));
// 	Style->Set("PMSEditor.AssetColors.Emitter", FLinearColor(1.0f, 0.3f, 0.0f));
// 	Style->Set("PMSEditor.AssetColors.Script", FLinearColor(1.0f, 1.0f, 0.0f));
// 	Style->Set("PMSEditor.AssetColors.ParameterCollection", FLinearColor(1.0f, 1.0f, 0.3f));
// 	Style->Set("PMSEditor.AssetColors.ParameterCollectionInstance", FLinearColor(1.0f, 1.0f, 0.7f));
// 	Style->Set("PMSEditor.AssetColors.ParameterDefinitions", FLinearColor(0.57f, 0.82f, 0.06f));
// 	Style->Set("PMSEditor.AssetColors.EffectType", FLinearColor(1.f, 1.f, 1.f));
// }

void InitClassIcon(TSharedRef< FSlateStyleSet > Style)
{
	// Renderer class icons
	// Style->Set("ClassIcon.PMSSpriteRendererProperties", new IMAGE_PLUGIN_BRUSH("Icons/Renderers/renderer_sprite", Icon16x16));
	// Style->Set("ClassIcon.PMSMeshRendererProperties", new IMAGE_PLUGIN_BRUSH("Icons/Renderers/renderer_mesh", Icon16x16));
	// Style->Set("ClassIcon.PMSRibbonRendererProperties", new IMAGE_PLUGIN_BRUSH("Icons/Renderers/renderer_ribbon", Icon16x16));
	// Style->Set("ClassIcon.PMSLightRendererProperties", new IMAGE_PLUGIN_BRUSH("Icons/Renderers/renderer_light", Icon16x16));
	// Style->Set("ClassIcon.PMSRendererProperties", new IMAGE_PLUGIN_BRUSH("Icons/Renderers/renderer_default", Icon16x16));
}

void InitButtonStyle(TSharedRef< FSlateStyleSet > Style)
{
	const FMargin FlagMargin = FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f);
	Style->Set("PMSEditor.NodeFlagBrushes.Left",new BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin));
	Style->Set("PMSEditor.NodeFlagBrushes.Center",new BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin));
	Style->Set("PMSEditor.NodeFlagBrushes.Right",new BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin));

	const FMargin FlagPadding = FMargin(14.0f,29.0f);
	const FCheckBoxStyle BypassFlagStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(1.000000f,1.000000f,1.000000f,0.000000f)))
		.SetUncheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.982251f,0.938686f,0.679542f)))
		.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.854993f,0.799103f,0.508881f)))
		// .SetUndeterminedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.982251f,0.938686f,0.679542f)))
		// .SetUndeterminedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.799103f,0.610496f,0.028426f)))
		// .SetUndeterminedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(1.000000f,0.846873f,0.215861f)))
		.SetCheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.799103f,0.610496f,0.028426f)))
		.SetCheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(1.000000f,0.846873f,0.215861f)))
		.SetCheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Left",FlagMargin,FLinearColor(0.610496f,0.467784f,0.023153f)))
		.SetPadding(FlagPadding);
	Style->Set("PMSEditor.NodeFlags.Bypass",BypassFlagStyle);

	const FCheckBoxStyle LockFlagStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,1.000000f,1.000000f,0.000000f)))
		.SetUncheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.913099f,0.520996f,0.439657f)))
		.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.863157f,0.508881f,0.439657f)))
		// .SetUndeterminedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.783538f,0.027321f,0.000000f)))
		// .SetUndeterminedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,0.033105f,0.000000f)))
		// .SetUndeterminedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.623960f,0.022174f,0.000000f)))
		.SetCheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.783538f,0.027321f,0.000000f)))
		.SetCheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,0.033105f,0.000000f)))
		.SetCheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.623960f,0.022174f,0.000000f)))
		.SetPadding(FlagPadding);
	Style->Set("PMSEditor.NodeFlags.Lock",LockFlagStyle);

	const FCheckBoxStyle TemplateFlagStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,1.000000f,1.000000f,0.000000f)))
		.SetUncheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.982251f,0.799103f,0.964686f)))
		.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.830770f,0.644480f,0.822786f)))
		// .SetUndeterminedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.723055f,0.205079f,0.679542f)))
		// .SetUndeterminedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,0.467784f,0.955973f)))
		// .SetUndeterminedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.545724f,0.158961f,0.514918f)))
		.SetCheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.723055f,0.205079f,0.679542f)))
		.SetCheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(1.000000f,0.467784f,0.955973f)))
		.SetCheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.545724f,0.158961f,0.514918f)))
		.SetPadding(FlagPadding);
	Style->Set("PMSEditor.NodeFlags.Template",TemplateFlagStyle);

	const FCheckBoxStyle DisplayFlagStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(1.000000f,1.000000f,1.000000f,0.000000f)))
		.SetUncheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.508881f,0.651406f,0.863157f)))
		.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.514918f,0.679542f,0.913099f)))
		// .SetUndeterminedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.417875f,0.871367f)))
		// .SetUndeterminedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.072272f,0.617207f,1.000000f)))
		// .SetUndeterminedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.327778f,0.672443f)))
		.SetCheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.417875f,0.871367f)))
		.SetCheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.072272f,0.617207f,1.000000f)))
		.SetCheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.327778f,0.672443f)))
		.SetPadding(FlagPadding);
	Style->Set("PMSEditor.NodeFlags.Display",DisplayFlagStyle);
	
	// //Style->Set("PMSEditor.NodeFlags.Left",FButtonStyle()
	// 	.Set
	// 	)
}

} // namespace PMSEditorStyleImpl

TSharedRef< FSlateStyleSet > FPMSEditorStyle::Create()
{
	using namespace PMSEditorStyleImpl;

	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PMSEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PMS")->GetBaseDir() / TEXT("Resources"));

	InitNodeIcons(Style);
	InitButtonStyle(Style);
	
	return Style;
}

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_CORE_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef BOX_CORE_BRUSH
#undef DEFAULT_FONT
#undef IMAGE_BRUSH_SVG

void FPMSEditorStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FPMSEditorStyle::Get()
{
	return *PMSEditorStyleInstance;
}
