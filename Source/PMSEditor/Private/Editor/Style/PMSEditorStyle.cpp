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
#include "Styling/ToolBarStyle.h"
// #include "Styling/SlateTypes.h"

TSharedPtr< FSlateStyleSet > FPMSEditorStyle::PMSEditorStyleInstance = NULL;
const float InputFocusRadius = 4.f;
const float InputFocusThickness = 1.0f;

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
#define IMAGE_BRUSH_SVG( RelativePath, ... ) FSlateVectorImageBrush( Style->RootToContentDir(RelativePath, TEXT(".svg")), __VA_ARGS__)
#define IMAGE_BRUSH_SVG_ABS( AbsolutePath, ... ) FSlateVectorImageBrush(AbsolutePath, __VA_ARGS__)

#define FONT(...) FSlateFontInfo(FCoreStyle::GetDefaultFont(), __VA_ARGS__)

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
		IFileManager::Get().FindFilesRecursive(SvgIcons,*(Style->GetContentRootDir()/TEXT("Icons")),TEXT("*.svg"),true,true);
		for (FString SvgIcon : SvgIcons)
		{
			FString StyleName = FString("PMSEditor.NodeIcons.") + FPaths::GetPathLeaf(FPaths::GetPath(SvgIcon)) +"_"+ FPaths::GetBaseFilename(SvgIcon);
			Style->Set(FName(StyleName),new IMAGE_BRUSH_SVG_ABS(SvgIcon,Icon64x64));
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
			.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Center",FlagMargin,FLinearColor(0.696000f,0.398478f,0.340344f)))
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
			.SetUncheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.338175f,0.473679f,0.675000f)))
			// .SetUndeterminedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.417875f,0.871367f)))
			// .SetUndeterminedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.072272f,0.617207f,1.000000f)))
			// .SetUndeterminedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.327778f,0.672443f)))
			.SetCheckedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.417875f,0.871367f)))
			.SetCheckedHoveredImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.072272f,0.617207f,1.000000f)))
			.SetCheckedPressedImage(BOX_PLUGIN_BRUSH("NodeBrushes/StateNode_Node_Button_Right",FlagMargin,FLinearColor(0.000000f,0.327778f,0.672443f)))
			.SetPadding(FlagPadding);
		Style->Set("PMSEditor.NodeFlags.Display",DisplayFlagStyle);


		
		// const FButtonStyle BorderlessButton = FButtonStyle(GetWidgetStyle<FButtonStyle>("SimpleButton"))
		// 		.SetNormalPadding(0.f)
		// 		.SetPressedPadding(0.f);
		
		// //Style->Set("PMSEditor.NodeFlags.Left",FButtonStyle()
		// 	.Set
		// 	)
	}
	void InitColorStyle(TSharedRef< FSlateStyleSet > Style)
	{
		/*Connection Color*/
		Style->Set("PMSEditor.ConnectionColor.Normal", FLinearColor(0.35f, 0.55f, 0.8f,0.8f));
		Style->Set("PMSEditor.ConnectionColor.Hover", FLinearColor(0.4f, 0.65f, 0.9f,1.0f));
	}
	void InitNavigationBarStyle(TSharedRef< FSlateStyleSet > Style)
	{
		FString NavigationBarFolderPath = Style->GetContentRootDir()/TEXT("HirechyNavigation");
		FString NavigationBarArrow = NavigationBarFolderPath/TEXT("Test.png");
		FString StyleName = FString("PMSEditor.NavigationBar.Arrow");
		Style->Set(FName(StyleName),new IMAGE_BRUSH_SVG_ABS(NavigationBarArrow,Icon64x64));
		
	}
	void InitToolBarStyle(TSharedRef<FSlateStyleSet> Style)
	{
		FToolBarStyle PMSToolBarStyle = FEditorStyle::Get().GetWidgetStyle<FToolBarStyle>("Menu");
		PMSToolBarStyle
			.SetButtonPadding(FMargin(10))
			.SetComboButtonPadding(FMargin(10))
			.SetCheckBoxPadding(FMargin(10))
			.SetIconSize(Icon20x20)
			.SetLabelPadding(FMargin(15))
			.SetBackgroundPadding(FMargin(20))
			.SetBlockPadding(FMargin(5))
		;
		Style->Set("PMSEditor.PMSToolMenu",PMSToolBarStyle);
	}
	void InitToolMenuStyle(TSharedPtr<FSlateStyleSet> Style)
	{
		Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));
		
		const FEditableTextBoxStyle& NormalEditableTextBoxStyle = FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
		const FTextBlockStyle& NormalText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
		const FTextBlockStyle& SmallButtonText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallButtonText");

		const FSlateColor SelectionColor = FEditorStyle::Get().GetSlateColor("SelectionColor");
		const FSlateColor SelectionColor_Pressed = FEditorStyle::Get().GetSlateColor("SelectionColor_Pressed");
		const FSlateColor DefaultForeground = FEditorStyle::Get().GetSlateColor("SelectionColor_Pressed");
		
		
		
		Style->Set("PMSMenu.WidgetBorder", new FSlateRoundedBoxBrush(FStyleColors::Input, 5.0f));
		FSpinBoxStyle SpinBox = FEditorStyle::GetWidgetStyle<FSpinBoxStyle>("Menu.SpinBox");
		Style->Set("PMSMenu.SpinBox", SpinBox);
		
		Style->Set("PMSMenu.Background", new FSlateColorBrush(FStyleColors::Dropdown));
		Style->Set("PMSMenu.Outline", new FSlateRoundedBoxBrush(FStyleColors::Transparent, 0.0f, FStyleColors::DropdownOutline, 1.f));
	
		Style->Set("PMSMenu.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
		Style->Set("PMSMenu.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
		Style->Set("PMSMenu.SubMenuIndicator", new IMAGE_BRUSH_SVG("Starship/Common/chevron-right", Icon16x16, FStyleColors::Foreground));
		// Style->Set("PMSMenu.SubMenuIndicator", new FSlateVectorImageBrush(Style->RootToContentDir("Starship/Common/chevron-right", TEXT(".svg")), Icon16x16, FStyleColors::Foreground));		Style->Set("PMSMenu.SToolBarComboButtonBlock.Padding", FMargin(2.0f));
		
		Style->Set("PMSMenu.SToolBarButtonBlock.Padding", FMargin(2.0f));
		Style->Set("PMSMenu.SToolBarCheckComboButtonBlock.Padding", FMargin(2.0f));
		Style->Set("PMSMenu.SToolBarButtonBlock.CheckBox.Padding", FMargin(0.0f));
		Style->Set("PMSMenu.SToolBarComboButtonBlock.ComboButton.Color", DefaultForeground);
		// Style->Set("PMSMenu.MenuIconSize", 14.f);
		Style->Set("PMSMenu.MenuIconSize", 20.f);

		const FMargin MenuBlockPadding(12.0f, 1.0f, 5.0f, 1.0f);
		Style->Set("PMSMenu.Block.IndentedPadding", MenuBlockPadding + FMargin(18.0f, 0, 0, 0));
		Style->Set("PMSMenu.Block.Padding", MenuBlockPadding);

		Style->Set("PMSMenu.Separator", new FSlateColorBrush(FStyleColors::White25));
		Style->Set("PMSMenu.Separator.Padding", FMargin(12.0f, 6.f, 12.0f, 6.f));

		Style->Set("PMSMenu.Label", NormalText);
		
		FEditableTextBoxStyle EditableText = FEditorStyle::GetWidgetStyle<FEditableTextBoxStyle>("Menu.EditableText");
		Style->Set("PMSMenu.EditableText", EditableText);
		FTextBlockStyle Keybinding = FEditorStyle::GetWidgetStyle<FTextBlockStyle>("Menu.Keybinding");
		Style->Set("PMSMenu.Keybinding", Keybinding);


		FSlateFontInfo XSFont(FONT(7, "Bold"));
		XSFont.LetterSpacing =  250;

		Style->Set("PMSMenu.Heading",
			FTextBlockStyle(SmallButtonText)
			.SetFont(XSFont)
			.SetColorAndOpacity(FStyleColors::White25));
		Style->Set("PMSMenu.Heading.Padding", FMargin(12.0f, 6.f, 12.f, 6.f));

		/* Set images for various SCheckBox states associated with menu check box items... */
		FLinearColor Transparent20 = FLinearColor(1.0, 1.0, 1.0, 0.2);
		FLinearColor Transparent01 = FLinearColor(1.0, 1.0, 1.0, 0.01);
		// const FCheckBoxStyle BasicMenuCheckBoxStyle = FCheckBoxStyle()
		// 	.SetUncheckedImage(           IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent01))
		// 	.SetUncheckedHoveredImage(    IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))
		// 	.SetUncheckedPressedImage(    IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))
		//
		// 	.SetCheckedImage(             IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))
		// 	.SetCheckedHoveredImage(      IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))
		// 	.SetCheckedPressedImage(      IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))
		//
		// 	.SetUndeterminedImage(        IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent01))
		// 	.SetUndeterminedHoveredImage( IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))
		// 	.SetUndeterminedPressedImage( IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20));
		
		
		/* ...and add the new style */
		FCheckBoxStyle BasicMenuCheckBoxStyle = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("Menu.CheckBox");
		Style->Set("PMSMenu.CheckBox", BasicMenuCheckBoxStyle);
		BasicMenuCheckBoxStyle = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("Menu.Check");
		Style->Set("PMSMenu.Check", BasicMenuCheckBoxStyle);

		/* This radio button is actually just a check box with different images */
		/* Set images for various Menu radio button (SCheckBox) states... */
		// const FCheckBoxStyle BasicMenuRadioButtonStyle = FCheckBoxStyle()
		// 	.SetUncheckedImage(           FSlateRoundedBoxBrush(FStyleColors::Input,  Icon8x8))
		// 	.SetUncheckedHoveredImage(    FSlateRoundedBoxBrush(FStyleColors::Input, Icon8x8))
		// 	.SetUncheckedPressedImage(    FSlateRoundedBoxBrush(FStyleColors::Hover2,   Icon8x8))
		// 	.SetCheckedImage(             FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
		// 	.SetCheckedHoveredImage(      FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
		// 	.SetCheckedPressedImage(      FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
		// 	.SetUndeterminedImage(        FSlateRoundedBoxBrush(FStyleColors::Input,  Icon8x8))
		// 	.SetUndeterminedHoveredImage( FSlateRoundedBoxBrush(FStyleColors::White, Icon8x8))
		// 	.SetUndeterminedPressedImage( FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8));

		/* ...and set new style */
		FCheckBoxStyle BasicMenuRadioButtonStyle = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("Menu.RadioButton");
		Style->Set("PMSMenu.RadioButton", BasicMenuRadioButtonStyle);
		/* Create style for "Menu.ToggleButton" widget ... */
		// const FCheckBoxStyle MenuToggleButtonCheckBoxStyle = FCheckBoxStyle()
		// 	.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		// 	.SetUncheckedImage(FSlateNoResource())
		// 	.SetUncheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 	.SetUncheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor))
		// 	.SetCheckedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 	.SetCheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 	.SetCheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor));

		/* ... and add new style */
		FCheckBoxStyle MenuToggleButtonCheckBoxStyle = FEditorStyle::GetWidgetStyle<FCheckBoxStyle>("Menu.ToggleButton");
		Style->Set("PMSMenu.ToggleButton", MenuToggleButtonCheckBoxStyle);

		// FButtonStyle MenuButton =
		// 	FButtonStyle(Style->GetWidgetStyle<FButtonStyle>("NoBorder"))
		// 	.SetNormal(FSlateNoResource())
		// 	.SetPressed(FSlateColorBrush(FStyleColors::Primary))
		// 	.SetHovered(FSlateColorBrush(FStyleColors::Primary))
		// 	.SetHoveredForeground(FStyleColors::ForegroundHover)
		// 	.SetNormalPadding(FMargin(0, 2))
		// 	.SetPressedPadding(FMargin(0, 3, 0, 1));

		FButtonStyle MenuButton = FEditorStyle::GetWidgetStyle<FButtonStyle>("Menu.Button");
		MenuButton
			.SetNormalPadding(FMargin(0))
			.SetPressedPadding(FMargin(0));
		
		Style->Set("PMSMenu.Button", MenuButton);

		Style->Set("PMSMenu.Button.Checked", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed));

		/* The style of a menu bar button when it has a sub menu open */
		Style->Set("PMSMenu.Button.SubMenuOpen", new BORDER_BRUSH("Common/Selection", FMargin(4.f / 16.f), FLinearColor(0.10f, 0.10f, 0.10f)));


		FButtonStyle MenuBarButton =
		FButtonStyle(MenuButton)
		.SetHovered(FSlateColorBrush(FStyleColors::Hover))
		.SetHoveredForeground(FStyleColors::ForegroundHover)
		.SetPressedForeground(FStyleColors::ForegroundHover)
		.SetNormalForeground(FStyleColors::Foreground);

		// For menu bars we need to ignore the button style

		Style->Set("WindowMenuBar.Background", new FSlateNoResource());
		Style->Set("WindowMenuBar.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
		Style->Set("WindowMenuBar.Button", MenuBarButton);		
		Style->Set("WindowMenuBar.Button.SubMenuOpen", new FSlateColorBrush(FStyleColors::Primary));
		Style->Set("WindowMenuBar.MenuBar.Padding", FMargin(12, 4));


		
		Style->SetContentRoot(IPluginManager::Get().FindPlugin("PMS")->GetBaseDir() / TEXT("Resources"));
	}
	void InitToolMenuStyleCopy(TSharedPtr<FSlateStyleSet> Style)
	{
		Style->SetContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

		const FEditableTextBoxStyle& NormalEditableTextBoxStyle = FEditorStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("NormalEditableTextBox");
		const FTextBlockStyle& NormalText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");
		const FTextBlockStyle& SmallButtonText = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SmallButtonText");

		const FSlateColor SelectionColor = FEditorStyle::Get().GetSlateColor("SelectionColor");
		const FSlateColor SelectionColor_Pressed = FEditorStyle::Get().GetSlateColor("SelectionColor_Pressed");
		const FSlateColor DefaultForeground = FEditorStyle::Get().GetSlateColor("SelectionColor_Pressed");

		// MultiBox
		{
			Style->Set("MultiBox.GenericToolBarIcon", new IMAGE_BRUSH("Icons/icon_generic_toolbar", Icon40x40));
			Style->Set("MultiBox.GenericToolBarIcon.Small", new IMAGE_BRUSH("Icons/icon_generic_toolbar", Icon20x20));

			Style->Set("MultiboxHookColor", FLinearColor(0.f, 1.f, 0.f, 1.f));
		}

		// ToolBar
		// {
		// 	FToolBarStyle NormalToolbarStyle =
		// 		FToolBarStyle()
		// 		.SetBackground(FSlateColorBrush(FStyleColors::Panel))
		// 		.SetBackgroundPadding(FMargin(0.f, 4.f, 0.f, 4.f))
		// 		.SetExpandBrush(IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16))
		// 		.SetComboButtonPadding(FMargin(4.0f, 0.0f))
		// 		.SetButtonPadding(FMargin(2.0f, 0.f))
		// 		.SetCheckBoxPadding(FMargin(4.0f, 0.f))
		// 		.SetSeparatorBrush(FSlateColorBrush(FStyleColors::Background))
		// 		.SetSeparatorPadding(FMargin(1.f, 0.f, 1.f, 0.f))
		// 		.SetLabelStyle(FTextBlockStyle(NormalText).SetFont(FStyleFonts::Get().Normal))
		// 		.SetEditableTextStyle(FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(FStyleFonts::Get().Normal))
		// 		.SetComboButtonStyle(Style->GetWidgetStyle<FComboButtonStyle>("ComboButton"))
		// 		.SetBlockPadding(FMargin(2.0f, 2.0f, 4.0f, 4.0f))
		// 		.SetIndentedBlockPadding(FMargin(18.0f, 2.0f, 4.0f, 4.0f));
		//
		//
		// 	/* Create style for "ToolBar.ToggleButton" widget ... */
		// 	const FCheckBoxStyle ToolBarToggleButtonCheckBoxStyle = FCheckBoxStyle()
		// 		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		// 		.SetUncheckedImage(FSlateNoResource())
		// 		.SetUncheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 		.SetUncheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor))
		// 		.SetCheckedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 		.SetCheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 		.SetCheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor));
		//
		// 	NormalToolbarStyle.SetToggleButtonStyle(ToolBarToggleButtonCheckBoxStyle);
		//
		// 	const FButtonStyle ToolbarButton = FButtonStyle()
		// 		.SetNormal(FSlateNoResource())
		// 		.SetPressed(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
		// 		.SetHovered(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor))
		// 		.SetNormalPadding(FMargin(2.f, 2.f, 2.f, 2.f))
		// 		.SetPressedPadding(FMargin(2.f, 3.f, 2.f, 1.f))
		// 		.SetNormalForeground(FSlateColor::UseForeground())
		// 		.SetPressedForeground(FSlateColor::UseForeground())
		// 		.SetHoveredForeground(FSlateColor::UseForeground())
		// 		.SetDisabledForeground(FSlateColor::UseForeground());
		//
		// 	NormalToolbarStyle.SetButtonStyle(ToolbarButton);
		// 	NormalToolbarStyle.SetIconSize(Icon40x40);
		//
		// 	Style->Set("ToolBar", NormalToolbarStyle);
		//
		// 	// Slim Toolbar
		// 	const FButtonStyle& SimpleButtonStyle = Style->GetWidgetStyle<FButtonStyle>("SimpleButton");
		//
		// 	FButtonStyle SlimToolBarButton = FButtonStyle(SimpleButtonStyle)
		// 		.SetNormalForeground(FStyleColors::Foreground)
		// 		.SetPressedForeground(FStyleColors::ForegroundHover)
		// 		.SetHoveredForeground(FStyleColors::ForegroundHover)
		// 		.SetDisabledForeground(FStyleColors::Foreground)
		// 		.SetNormalPadding(FMargin(8.f, 4.f, 8.f, 4.f))
		// 		.SetPressedPadding(FMargin(8.f, 5.f, 8.f, 3.f));
		//
		//
		// 	FComboButtonStyle SlimToolBarComboButton = FComboButtonStyle(Style->GetWidgetStyle<FComboButtonStyle>("SimpleComboButton"))
		// 		.SetDownArrowPadding(FMargin(1.f, 0.f, 2.f, 0.f));
		//
		// 	FToolBarStyle SlimToolbarStyle =
		// 		FToolBarStyle()
		// 		.SetBackground(FSlateColorBrush(FStyleColors::Panel))
		// 		.SetBackgroundPadding(FMargin(4.f, 6.f, 0.f, 6.f))
		// 		.SetExpandBrush(IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16))
		// 		.SetComboButtonPadding(FMargin(0.0f, 0.0f))
		// 		.SetButtonPadding(FMargin(4.0f, 0.0f))
		// 		.SetCheckBoxPadding(FMargin(8.0f, 0.0f))
		// 		.SetSeparatorBrush(FSlateColorBrush(FStyleColors::Recessed))
		// 		.SetSeparatorPadding(FMargin(4.f, -5.0f))
		// 		.SetLabelStyle(FTextBlockStyle(NormalText))
		// 		.SetComboButtonStyle(SlimToolBarComboButton)
		// 		.SetLabelPadding(FMargin(4.f, 0.f, 0.f, 0.f))
		// 		.SetEditableTextStyle(FEditableTextBoxStyle(NormalEditableTextBoxStyle));
		//
		// 	FCheckBoxStyle SlimToolBarToggleButtonCheckBoxStyle = FCheckBoxStyle(Style->GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckbox"))
		// 		.SetPadding(FMargin(8.f, 4.f, 8.f, 4.f));
		//
		// 	SlimToolbarStyle.SetToggleButtonStyle(SlimToolBarToggleButtonCheckBoxStyle);
		// 	SlimToolbarStyle.SetButtonStyle(SlimToolBarButton);
		//
		// 	FButtonStyle SlimToolBarButtonSettingsRegion = FButtonStyle(SimpleButtonStyle)
		// 		.SetPressed(FSlateNoResource())
		// 		.SetHovered(FSlateNoResource())
		// 		.SetNormal(FSlateNoResource())
		// 		.SetNormalForeground(FStyleColors::Foreground)
		// 		.SetPressedForeground(FStyleColors::ForegroundHover)
		// 		.SetHoveredForeground(FStyleColors::ForegroundHover)
		// 		.SetDisabledForeground(FStyleColors::Foreground)
		// 		.SetNormalPadding(FMargin(8.f, 4.f, 8.f, 4.f))
		// 		.SetPressedPadding(FMargin(8.f, 5.f, 8.f, 3.f));
		//
		//
		// 	FCheckBoxStyle SettingsToggleButtonStyle = FCheckBoxStyle(Style->GetWidgetStyle<FCheckBoxStyle>("TransparentCheckBox"))
		// 		.SetPadding(FMargin(8.f, 4.f, 8.f, 4.f));
		//
		// 	SlimToolbarStyle.SetSettingsButtonStyle(SlimToolBarButtonSettingsRegion);
		// 	SlimToolbarStyle.SetSettingsToggleButtonStyle(SettingsToggleButtonStyle);
		// 	SlimToolbarStyle.SetIconSize(Icon20x20);
		//
		// 	SlimToolBarButtonSettingsRegion
		// 		.SetNormalPadding(FMargin(4.f, 4.f, 4.f, 4.f))
		// 		.SetPressedPadding(FMargin(4.f, 5.f, 4.f, 3.f));
		//
		// 	FComboButtonStyle SlimToolBarSettingsComboButton = FComboButtonStyle(SlimToolBarComboButton)
		// 		.SetContentPadding(0)
		// 		.SetButtonStyle(SlimToolBarButtonSettingsRegion)
		// 		.SetDownArrowImage(IMAGE_BRUSH_SVG("Starship/Common/ellipsis-vertical-narrow", FVector2D(6, 20)));
		//
		// 	SlimToolbarStyle.SetSettingsComboButtonStyle(SlimToolBarSettingsComboButton);
		//
		//
		// 	Style->Set("ToolbarSettingsRegion.LeftHover", new FSlateRoundedBoxBrush(FStyleColors::Hover, FVector4(4.0f, 0.0f, 0.0f, 4.0f)));
		// 	Style->Set("ToolbarSettingsRegion.RightHover", new FSlateRoundedBoxBrush(FStyleColors::Hover, FVector4(0.0f, 4.0f, 4.0f, 0.0f)));
		// 	Style->Set("ToolbarSettingsRegion.Left", new FSlateRoundedBoxBrush(FStyleColors::Dropdown, FVector4(4.0f, 0.0f, 0.0f, 4.0f), FStyleColors::Header, 1.0f));
		// 	Style->Set("ToolbarSettingsRegion.Right", new FSlateRoundedBoxBrush(FStyleColors::Dropdown, FVector4(0.0f, 4.0f, 4.0f, 0.0f), FStyleColors::Header, 1.0f));
		// 	Style->Set("ToolbarSettingsRegion.LeftToggleHover", new FSlateRoundedBoxBrush(FStyleColors::PrimaryHover, FVector4(4.0f, 0.0f, 0.0f, 4.0f)));
		// 	Style->Set("ToolbarSettingsRegion.LeftToggle", new FSlateRoundedBoxBrush(FStyleColors::Primary, FVector4(4.0f, 0.0f, 0.0f, 4.0f)));
		//
		//
		// 	Style->Set("SlimToolBar", SlimToolbarStyle);
		//
		//
		// 	SlimToolbarStyle.SetShowLabels(false);
		// 	
		// 	Style->Set("AssetEditorToolbar", SlimToolbarStyle);
		//
		// 	// Callout Toolbar - Used to "call out" the toolbar button with text
		// 	{
		// 		SlimToolbarStyle.SetShowLabels(true);
		//
		// 		Style->Set("CalloutToolbar", SlimToolbarStyle);
		// 	}
		// }

		// MenuBar
		{
			Style->Set("PMSMenu.WidgetBorder", new FSlateRoundedBoxBrush(FStyleColors::Input, 5.0f));
			Style->Set("PMSMenu.SpinBox", FSpinBoxStyle()
				.SetBackgroundBrush(FSlateRoundedBoxBrush(FStyleColors::Input, InputFocusRadius, FStyleColors::Secondary, InputFocusThickness))
				.SetHoveredBackgroundBrush(FSlateRoundedBoxBrush(FStyleColors::Input, InputFocusRadius, FStyleColors::Hover, InputFocusThickness))

				.SetActiveFillBrush(FSlateRoundedBoxBrush(FStyleColors::Hover, InputFocusRadius, FLinearColor::Transparent, InputFocusThickness))
				.SetInactiveFillBrush(FSlateRoundedBoxBrush(FStyleColors::Secondary, InputFocusRadius, FLinearColor::Transparent, InputFocusThickness))
				.SetArrowsImage(FSlateNoResource())
				.SetForegroundColor(FStyleColors::ForegroundHover)
				.SetTextPadding(FMargin(10.f, 3.5f, 10.f, 4.f))
			);


			Style->Set("PMSMenu.Background", new FSlateColorBrush(FStyleColors::Dropdown));
			Style->Set("PMSMenu.Outline", new FSlateRoundedBoxBrush(FStyleColors::Transparent, 0.0f, FStyleColors::DropdownOutline, 1.f));
			Style->Set("PMSMenu.Icon", new IMAGE_BRUSH("Icons/icon_tab_toolbar_16px", Icon16x16));
			Style->Set("PMSMenu.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
			Style->Set("PMSMenu.SubMenuIndicator", new IMAGE_BRUSH_SVG("Starship/Common/chevron-right", Icon16x16, FStyleColors::Foreground));
			Style->Set("PMSMenu.SToolBarComboButtonBlock.Padding", FMargin(2.0f));
			Style->Set("PMSMenu.SToolBarButtonBlock.Padding", FMargin(2.0f));
			Style->Set("PMSMenu.SToolBarCheckComboButtonBlock.Padding", FMargin(2.0f));
			Style->Set("PMSMenu.SToolBarButtonBlock.CheckBox.Padding", FMargin(0.0f));
			Style->Set("PMSMenu.SToolBarComboButtonBlock.ComboButton.Color", DefaultForeground);
			Style->Set("PMSMenu.MenuIconSize", 16.f);

			const FMargin MenuBlockPadding(1.0f, 0.0f, 1.0f, 0.0f);
			Style->Set("PMSMenu.Block.IndentedPadding", MenuBlockPadding + FMargin(18.0f, 0, 0, 0));
			Style->Set("PMSMenu.Block.Padding", MenuBlockPadding);/*Used*/

			Style->Set("PMSMenu.Separator", new FSlateColorBrush(FStyleColors::White25));
			Style->Set("PMSMenu.Separator.Padding", FMargin(12.0f, 6.f, 12.0f, 6.f));

			Style->Set("PMSMenu.Label", NormalText);

			Style->Set("PMSMenu.EditableText", FEditableTextBoxStyle(NormalEditableTextBoxStyle).SetFont(FStyleFonts::Get().Normal));
			Style->Set("PMSMenu.Keybinding", FTextBlockStyle(NormalText).SetFont(FStyleFonts::Get().Small));


			FSlateFontInfo XSFont(FONT(8, "Bold"));
			XSFont.LetterSpacing =  250;

			Style->Set("PMSMenu.Heading",
				FTextBlockStyle(SmallButtonText)
				.SetFont(XSFont)
				.SetColorAndOpacity(FStyleColors::White25));
			Style->Set("PMSMenu.Heading.Padding", FMargin(12.0f, 6.f, 12.f, 6.f));

			/* Set images for various SCheckBox states associated with menu check box items... */
			FLinearColor Transparent20 = FLinearColor(1.0, 1.0, 1.0, 0.2);
			FLinearColor Transparent01 = FLinearColor(1.0, 1.0, 1.0, 0.01);
			const FCheckBoxStyle BasicMenuCheckBoxStyle = FCheckBoxStyle()
				.SetUncheckedImage(           IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent01))
				.SetUncheckedHoveredImage(    IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))
				.SetUncheckedPressedImage(    IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))

				.SetCheckedImage(             IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))
				.SetCheckedHoveredImage(      IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))
				.SetCheckedPressedImage(      IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16))

				.SetUndeterminedImage(        IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent01))
				.SetUndeterminedHoveredImage( IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20))
				.SetUndeterminedPressedImage( IMAGE_BRUSH_SVG("Starship/Common/check", Icon16x16, Transparent20));

			/* ...and add the new style */
			Style->Set("PMSMenu.CheckBox", BasicMenuCheckBoxStyle);
			Style->Set("PMSMenu.Check", BasicMenuCheckBoxStyle);

			/* This radio button is actually just a check box with different images */
			/* Set images for various Menu radio button (SCheckBox) states... */
			const FCheckBoxStyle BasicMenuRadioButtonStyle = FCheckBoxStyle()
				.SetUncheckedImage(           FSlateRoundedBoxBrush(FStyleColors::Input,  Icon8x8))
				.SetUncheckedHoveredImage(    FSlateRoundedBoxBrush(FStyleColors::Input, Icon8x8))
				.SetUncheckedPressedImage(    FSlateRoundedBoxBrush(FStyleColors::Hover2,   Icon8x8))
				.SetCheckedImage(             FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
				.SetCheckedHoveredImage(      FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
				.SetCheckedPressedImage(      FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8))
				.SetUndeterminedImage(        FSlateRoundedBoxBrush(FStyleColors::Input,  Icon8x8))
				.SetUndeterminedHoveredImage( FSlateRoundedBoxBrush(FStyleColors::White, Icon8x8))
				.SetUndeterminedPressedImage( FSlateRoundedBoxBrush(FStyleColors::White,   Icon8x8));

			/* ...and set new style */
			Style->Set("PMSMenu.RadioButton", BasicMenuRadioButtonStyle);

			/* Create style for "PMSMenu.ToggleButton" widget ... */
			const FCheckBoxStyle MenuToggleButtonCheckBoxStyle = FCheckBoxStyle()
				.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
				.SetUncheckedImage(FSlateNoResource())
				.SetUncheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
				.SetUncheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor))
				.SetCheckedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
				.SetCheckedHoveredImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed))
				.SetCheckedPressedImage(BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor));

			/* ... and add new style */
			Style->Set("PMSMenu.ToggleButton", MenuToggleButtonCheckBoxStyle);

			/*By Setting Different Padding To Response to button down*/
			FButtonStyle MenuButton =
				FButtonStyle(Style->GetWidgetStyle<FButtonStyle>("NoBorder"))
				.SetNormal(FSlateNoResource())
				.SetPressed(FSlateColorBrush(FStyleColors::Primary))
				.SetHovered(FSlateColorBrush(FStyleColors::Primary))
				.SetHoveredForeground(FStyleColors::ForegroundHover)
				.SetNormalPadding(FMargin(0, 1))
				.SetPressedPadding(FMargin(0, 2, 0, 0));

			Style->Set("PMSMenu.Button", MenuButton);

			Style->Set("PMSMenu.Button.Checked", new BOX_BRUSH("Common/RoundedSelection_16x", 4.0f / 16.0f, SelectionColor_Pressed));

			/* The style of a menu bar button when it has a sub menu open */
			Style->Set("PMSMenu.Button.SubMenuOpen", new BORDER_BRUSH("Common/Selection", FMargin(4.f / 16.f), FLinearColor(0.10f, 0.10f, 0.10f)));


			FButtonStyle MenuBarButton =
				FButtonStyle(MenuButton)
				.SetHovered(FSlateColorBrush(FStyleColors::Hover))
				.SetHoveredForeground(FStyleColors::ForegroundHover)
				.SetPressedForeground(FStyleColors::ForegroundHover)
				.SetNormalForeground(FStyleColors::Foreground);

			// For menu bars we need to ignore the button style

			Style->Set("WindowMenuBar.Background", new FSlateNoResource());
			Style->Set("WindowMenuBar.Label", FTextBlockStyle(NormalText).SetFont(FStyleFonts::Get().Normal));
			Style->Set("WindowMenuBar.Expand", new IMAGE_BRUSH("Icons/toolbar_expand_16x", Icon16x16));
			Style->Set("WindowMenuBar.Button", MenuBarButton);
			Style->Set("WindowMenuBar.Button.SubMenuOpen", new FSlateColorBrush(FStyleColors::Primary));
			Style->Set("WindowMenuBar.MenuBar.Padding", FMargin(12, 4));
		}
		Style->SetContentRoot(IPluginManager::Get().FindPlugin("PMS")->GetBaseDir() / TEXT("Resources"));
	}
} // namespace PMSEditorStyleImpl

TSharedRef< FSlateStyleSet > FPMSEditorStyle::Create()
{
	using namespace PMSEditorStyleImpl;

	/*Here the class static member PMSEditorStyleInstance will be set to TsharedRef<FSlateStyleSet> Style*/
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("PMSEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("PMS")->GetBaseDir() / TEXT("Resources"));

	InitNodeIcons(Style);
	InitButtonStyle(Style);
	InitColorStyle(Style);
	InitNavigationBarStyle(Style);
	InitToolBarStyle(Style);
	// InitToolMenuStyle(Style);
	InitToolMenuStyleCopy(Style);
	return Style;
}

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_CORE_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef BOX_CORE_BRUSH
#undef DEFAULT_FONT
#undef IMAGE_PLUGIN_BRUSH_SVG
#undef IMAGE_BRUSH_SVG
#undef IMAGE_BRUSH_SVG_ABS

void FPMSEditorStyle::ReloadTextures()
{
	FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FPMSEditorStyle::Get()
{
	return *PMSEditorStyleInstance;
}
