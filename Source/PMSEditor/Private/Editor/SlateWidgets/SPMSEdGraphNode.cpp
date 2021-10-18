#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

#include "Editor/PMSEdGraphNode.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Brushes/SlateImageBrush.h"

#include "Widgets/Layout/SScrollBox.h"

#include "HAL/FileManager.h"

//#include "Widgets/Input/"
#include "Slate.h"


//可以把所有图标用它那个search svg的东西画在左边，然后拖到场景中的方式
void SPMSEdGraphNode::Construct(const FArguments& InArgs, UPMSEdGraphNode* InNode)
{
	GraphNode = InNode;
    IconName = &(InNode->IconName);
    NodeMargin = &(InNode->Margin);
    NodeColor = &(InNode->Color);
	UpdateGraphNode();
}

auto LeftRightLabel = [](const FName& InIconName = FName(), const FText& InLabel = FText::GetEmpty(), const FName& InTextStyle = TEXT("ButtonText")) -> TSharedRef<SWidget>
{
	TSharedPtr<SHorizontalBox> HBox = SNew(SHorizontalBox);
	float Space = InIconName.IsNone() ? 0.0f : 8.f;

	if (!InIconName.IsNone())
	{
		HBox->AddSlot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(
				FAppStyle::Get().GetBrush(InIconName)
				//new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"), 1.f)
			)
			];
	}

	if (!InLabel.IsEmpty())
	{
		HBox->AddSlot()
			.VAlign(VAlign_Center)
			.Padding(Space, 0.5f, 0.f, 0.f)  // Compensate down for the baseline since we're using all caps
			.AutoWidth()
			[
				SNew(STextBlock)
				.TextStyle(&FAppStyle::Get().GetWidgetStyle< FTextBlockStyle >(InTextStyle))
			.Justification(ETextJustify::Center)
			.Text(InLabel)
			];
	}

	return SNew(SBox).HeightOverride(16.f)[HBox.ToSharedRef()];
};

auto NodeIcon = [](FString NodeName) -> TSharedRef<SImage>
{
	/*FString SearchDir = FPaths::ProjectPluginsDir().Append("HoudiniEdGraph/Resources/SOP/");
	TArray<FString> FoundIcons;
	IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDir, TEXT("*.svg"), true, false);*/
	FString SopDir = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/SOP/");
	FString NodeIconPath = SopDir.Append(NodeName);
	const FVector2D IconSize(30.f, 30.f);
	UE_LOG(LogTemp, Log, TEXT("%s"), *NodeIconPath);
    
	//return SNew(SImage)
	//	.Image(new FSlateVectorImageBrush(NodeIconPath, IconSize, FLinearColor(1, 1, 1, 1)));

    FSlateVectorImageBrush* VectorImageBrush1 = new FSlateVectorImageBrush(NodeIconPath, IconSize);
    return SNew(SImage)
        .Image(VectorImageBrush1);
};



/*
TSharedRef<SWidget> ConstructIconsGallery()
{
    // auto GenerateColorButton = [] (FTex      t InTitle, FName StyleColorName, bool Inverted = false) -> TSharedRef<SWidget> 

    auto GenerateIconLibrarySVG = [](FString InPath) -> TSharedRef<SWidget>
    {
        const FVector2D IconSize(20.f, 20.f);
        TSharedPtr<SUniformWrapPanel> UniformWrapPanel = SNew(SUniformWrapPanel)
            .HAlign(HAlign_Left)
            .SlotPadding(FMargin(12.f, 12.f));

        TArray<FString> FoundIcons;
        FString SearchDirectory = FPaths::ProjectPluginsDir().Append(InPath);// TEXT("Editor/Slate/Icons/GeneralTools");
        // IFileManager::Get().FindFiles(FoundIcons, *SearchDirectory, TEXT(".png"));//, true, true, false);
        IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDirectory, TEXT("*.svg"), true, false);
		TArray< TUniquePtr< FSlateBrush > > DynamicBrushes;
        for (const FString& Filename : FoundIcons)
        {
            // FString IconPath = SearchDirectory / Filename;
            FString IconPath = Filename;
			UE_LOG(LogTemp, Log, TEXT("%s"), *IconPath);
            DynamicBrushes.Add(TUniquePtr<FSlateVectorImageBrush>(new FSlateVectorImageBrush(IconPath, IconSize)));

            UniformWrapPanel->AddSlot()
                [
                    SNew(SImage)
                    .Image(DynamicBrushes.Last().Get())
                .ToolTipText(FText::FromString(IconPath))
                ];
        }

        return SNew(SVerticalBox)
        + SVerticalBox::Slot()
            [
                UniformWrapPanel.ToSharedRef()
            ];
    };

    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG("HoudiniEdGraph/Resources/SOP/test")]
        ];
}
*/
/*
TArray< TUniquePtr< FSlateBrush > > DynamicBrushes;
TSharedRef<SWidget> ConstructIconsGallery()
{
    // auto GenerateColorButton = [] (FTex      t InTitle, FName StyleColorName, bool Inverted = false) -> TSharedRef<SWidget> 

    auto GenerateIconLibrary = [](FText InTitle, FString InPath) -> TSharedRef<SWidget>
    {
        const FVector2D IconSize(20.f, 20.f);
        TSharedPtr<SUniformWrapPanel> UniformWrapPanel = SNew(SUniformWrapPanel)
            .HAlign(HAlign_Left)
            .SlotPadding(FMargin(12.f, 12.f));

        TArray<FString> FoundIcons;
        FString SearchDirectory = FPaths::EngineDir() / InPath;// TEXT("Editor/Slate/Icons/GeneralTools");
        // IFileManager::Get().FindFiles(FoundIcons, *SearchDirectory, TEXT(".png"));//, true, true, false);
        IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDirectory, TEXT("*.png"), true, false);
        for (const FString& Filename : FoundIcons)
        {
            // FString IconPath = SearchDirectory / Filename;
            FString IconPath = Filename;

            DynamicBrushes.Add(TUniquePtr<FSlateDynamicImageBrush>(new FSlateDynamicImageBrush(FName(*IconPath), IconSize)));

            UniformWrapPanel->AddSlot()
                [
                    SNew(SImage)
                    .Image(DynamicBrushes.Last().Get())
                .ToolTipText(FText::FromString(IconPath))
                ];
        }

        return SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock).Text(InTitle)
            ]

        + SVerticalBox::Slot()
            [
                UniformWrapPanel.ToSharedRef()
            ];
    };

    auto GenerateIconLibrarySVG = [](FText InTitle, FString InPath) -> TSharedRef<SWidget>
    {
        const FVector2D IconSize(20.f, 20.f);
        TSharedPtr<SUniformWrapPanel> UniformWrapPanel = SNew(SUniformWrapPanel)
            .HAlign(HAlign_Left)
            .SlotPadding(FMargin(12.f, 12.f));

        TArray<FString> FoundIcons;
        FString SearchDirectory = FPaths::EngineDir() / InPath;// TEXT("Editor/Slate/Icons/GeneralTools");
        // IFileManager::Get().FindFiles(FoundIcons, *SearchDirectory, TEXT(".png"));//, true, true, false);
        IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDirectory, TEXT("*.svg"), true, false);
        for (const FString& Filename : FoundIcons)
        {
            // FString IconPath = SearchDirectory / Filename;
            FString IconPath = Filename;

            DynamicBrushes.Add(TUniquePtr<FSlateVectorImageBrush>(new FSlateVectorImageBrush(IconPath, IconSize)));

            UniformWrapPanel->AddSlot()
                [
                    SNew(SImage)
                    .Image(DynamicBrushes.Last().Get())
                .ToolTipText(FText::FromString(IconPath))
                ];
        }

        return SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(STextBlock).Text(InTitle)
            ]

        + SVerticalBox::Slot()
            [
                UniformWrapPanel.ToSharedRef()
            ];
    };



    return SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("ToolPanel.GroupBorder"))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "SlateCore", "Core"), "Content/Slate/Starship/Common")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "Editor Common", "Editor"), "Content/Editor/Slate/Starship/Common")]

        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "SceneOutliner", "SceneOutliner"), "Content/Editor/Slate/Starship/SceneOutliner")]
        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "LevelEditor", "LevelEditor"), "Content/Editor/Slate/Starship/LevelEditor/Menus")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "MainToolbar", "MainToolbar"), "Content/Editor/Slate/Starship/MainToolbar")]
        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "FileMenu", "FileMenu"), "Content/Editor/Slate/Starship/Menus/File")]
        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "EditMenu", "EditMenu"), "Content/Editor/Slate/Starship/Menus/Edit")]
        // +SVerticalBox::Slot().AutoHeight()[ GenerateIconLibrarySVG(NSLOCTEXT("StarshipGallery", "HelpMenu", "HelpMenu"), "Content/Editor/Slate/Starship/Menus/Help")]

        +SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "PaintIconTitle", "Paint"), "Content/Editor/Slate/Icons/Paint")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "LandscapeIconTitle", "Landscape"), "Content/Editor/Slate/Icons/Landscape")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "ModelingIconTitle", "Modeling"), "/Plugins/Experimental/ModelingToolsEditorMode/Content/Icons")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "FractureIconTitle", "Fracture"), "/Plugins/Experimental/ChaosEditor/Content")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "CurveEditorIconTitle", "CurveEditor"), "Content/Editor/Slate/GenericCurveEditor/Icons")]
        + SVerticalBox::Slot().AutoHeight()[GenerateIconLibrary(NSLOCTEXT("StarshipGallery", "GeneralIconTitle", "General"), "Content/Editor/Slate/Icons/GeneralTools")]
        ];
}
*/



void SPMSEdGraphNode::UpdateGraphNode()
{
	GetOrAddSlot(ENodeZone::Center)
	[

		SNew(SBorder)
		//.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(FMargin(*NodeMargin))
		.BorderBackgroundColor(*NodeColor)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				/* + SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
					[
						LeftRightLabel("Icons.box-perspective")
					]
				]
                */
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                    .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
                //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
                .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                ]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
					//.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
					//.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
					.CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
					.CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
					.CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
					.UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
					.UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					NodeIcon(*IconName)
				]
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SCheckBox)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                    .CheckedImage(new FSlateColorBrush(FLinearColor(300.f,.65f,1.f).HSVToLinearRGB()))
                    .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
                    .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
                    .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
                    .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
                    ]
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SNew(SCheckBox)
                        .Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
                    //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                    //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                    .CheckedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Primary")))
                    .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                    .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                    .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                    .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                    ]
			]
		]
	];
}

void SPMSEdGraphNode::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty) {
    if (!NodeFilter.Find(SharedThis(this)))
    {
        if (GraphNode && !RequiresSecondPassLayout())
        {
            NodeFilter.Add(SharedThis(this));
            GraphNode->Modify(bMarkDirty);
            GraphNode->NodePosX = NewPosition.X;
            GraphNode->NodePosY = NewPosition.Y;
            
            UpdateGraphNode();
        }
    }
}