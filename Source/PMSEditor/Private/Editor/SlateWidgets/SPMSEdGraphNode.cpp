#include "Editor/SlateWidgets/SPMSEdGraphNode.h"

#include "Editor/PMSEdGraphNode.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Brushes/SlateImageBrush.h"

#include "Widgets/Layout/SScrollBox.h"

#include "HAL/FileManager.h"

//#include "Widgets/Input/"
#include "SGraphPin.h"
#include "Slate.h"
#include "Editor/SlateWidgets/SPMSEdGraphPin.h"

//Used in SetOwner
#include "GeomTools.h"
#include "MeshDescription.h"
//#include "RawMesh.h"

#include "SGraphPanel.h"
#include "Editor/PMSEdGraph.h"
#include "Slate/SlateVectorArtData.h"
#include "Editor/SlateWidgets/S2DMeshWidget.h"
#include "Json.h"
#include "Editor/Style/PMSEditorStyle.h"
//#include "Json/Public/Serialization/JsonSerializer.h"

#define insert 1

#if insert
#define INSERT_BLOCK\
			+ SHorizontalBox::Slot()\
			.AutoWidth()\
			.VAlign(VAlign_Fill)\
			.HAlign(HAlign_Fill)\
			.Padding(FMargin(0.0f,0.5f))\
			[\
				SNew(SBorder)\
				.BorderImage(new FSlateColorBrush(FLinearColor(0.3f,0.3f,0.3f)))\
				.ColorAndOpacity(FLinearColor(0.3f,0.3f,0.3f,1.0f))\
				.Padding(FMargin(1.0f,0.0f))\
			]
#endif

//Todo 此部分后续要变为SMeshWidget的Style
TArray<TArray<FClipSMTriangle>> StaticMeshFromJson(FString JsonFilePath)
{
	FString JsonStr;
	TArray<TArray<FClipSMTriangle>> OutGeo;
	bool bLoadSuccess = FFileHelper::LoadFileToString(JsonStr,*JsonFilePath);
	if(!bLoadSuccess)
	{
		return OutGeo;
	}
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	FJsonSerializer::Deserialize(JsonReader,JsonObject);
	//variable name is the same as its name in json
	FString name = JsonObject->GetStringField("name");
	TSharedPtr<FJsonObject> flags = JsonObject->GetObjectField("flags");
	TArray<TSharedPtr<FJsonValue>> outline = JsonObject->GetArrayField("outline");
	TArray<TSharedPtr<FJsonValue>> inputs = JsonObject->GetArrayField("inputs");
	TArray<TSharedPtr<FJsonValue>> outputs = JsonObject->GetArrayField("outputs");
	TArray<TSharedPtr<FJsonValue>> icon = JsonObject->GetArrayField("icon");
	//TArray<UStaticMesh> ShapeVectors;
	for(int i=0;i<4;i++)
	{
		TSharedPtr<FJsonObject> CurFlag = flags->GetObjectField(FString::FromInt(i));
		TArray<TSharedPtr<FJsonValue>> CurOutline = CurFlag->GetArrayField("outline");
		//FRawMesh FlagRawMesh;
		FString MeshName = "Flag"+FString::FromInt(i);
		//init FlagPolygon, param means the count of triangle?
		FClipSMPolygon FlagPolygon(CurOutline.Num()-2);
		TArray<FClipSMTriangle> FlagTriangles;
		//FlagRawMesh.VertexPositions.Add()
		//FlagRawMesh.WedgeIndices.Add();
		
		FVector2D MaxBound(CurOutline[0]->AsArray()[0]->AsNumber()*100,CurOutline[1]->AsArray()[0]->AsNumber()*100);
		FVector2D MinBound(MaxBound);
		for(TSharedPtr<FJsonValue> Point:CurOutline)
		{
			auto PointElementsArray = Point->AsArray();
			FClipSMVertex Flagvertex;
			Flagvertex.Pos = FVector3f(PointElementsArray[0]->AsNumber()*100,0,PointElementsArray[1]->AsNumber()*100);
			Flagvertex.Color = FColor::White;
			FlagPolygon.Vertices.Add(Flagvertex);
			MaxBound.X = FMath::Max(Flagvertex.Pos.X,MaxBound.X);
			MaxBound.Y = FMath::Max(Flagvertex.Pos.Z,MaxBound.Y);
			MinBound.X = FMath::Min(Flagvertex.Pos.X,MinBound.X);
			MinBound.Y = FMath::Min(Flagvertex.Pos.Z,MinBound.Y);
		}
		for(FClipSMVertex Flagvertex:FlagPolygon.Vertices)
		{
			Flagvertex.UVs[0].X = (Flagvertex.Pos.X-MinBound.X)/(MaxBound.X-MinBound.X);
			Flagvertex.UVs[1].X = Flagvertex.UVs[0].X;
			Flagvertex.UVs[2].X = Flagvertex.UVs[1].X;
			
			Flagvertex.UVs[0].Y = (Flagvertex.Pos.Z-MinBound.Y)/(MaxBound.Y-MinBound.Y);
			Flagvertex.UVs[1].Y = Flagvertex.UVs[0].Y;
			Flagvertex.UVs[2].Y = Flagvertex.UVs[1].Y;
		}
		//these triangles has unique points
		FGeomTools::TriangulatePoly(FlagTriangles,FlagPolygon);
		OutGeo.Add(FlagTriangles);
		// ;
		// int vtxid=0;
		// for(auto trin:FlagTriangles)
		// {
		// 	for(auto vtx:trin.Vertices)
		// 	{
		// 		FlagRawMesh.VertexPositions.Add(vtx.Pos);
		// 		FlagRawMesh.WedgeIndices.Add(vtxid);
		// 		//FlagRawMesh.WedgeColors.Add(FColor());
		// 		FlagRawMesh.WedgeTangentX.Add(vtx.TangentX);
		// 		FlagRawMesh.WedgeTangentY.Add(vtx.TangentY);
		// 		FlagRawMesh.WedgeTangentZ.Add(vtx.TangentZ);
		// 		FlagRawMesh.WedgeTexCoords->Add(vtx.UVs[0]);
		// 		vtxid++;
		// 	}
		// }
		// FStaticMeshSourceModel& SrcModel = FlagStaticMesh->AddSourceModel();
	}
	return OutGeo;
}


//Todo 查看InArgs里面有哪些东西
void SPMSEdGraphNode::Construct(const FArguments& InArgs, UPMSEdGraphNode* InNode)
{
	GraphNode = InNode;
    IconName = &(InNode->IconName);
    NodeMargin = &(InNode->Margin);
    NodeColor = &(InNode->Color);
	NodePadding = &(InNode->NodePadding);
	NodeSize = &(InNode->NodeSize);
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

auto NodeIcon = [](FString NodeName, FVector2D NodeMargin) -> TSharedRef<SImage>
{
	/*FString SearchDir = FPaths::ProjectPluginsDir().Append("HoudiniEdGraph/Resources/SOP/");
	TArray<FString> FoundIcons;
	IFileManager::Get().FindFilesRecursive(FoundIcons, *SearchDir, TEXT("*.svg"), true, false);*/
	FString SopDir = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/Icons/");
	FString NodeIconPath = SopDir.Append(NodeName);
	const FVector2D IconSize(48.f, 48.f);
	//UE_LOG(LogTemp, Log, TEXT("%s"), *NodeIconPath);
    
	//return SNew(SImage)
	//	.Image(new FSlateVectorImageBrush(NodeIconPath, IconSize, FLinearColor(1, 1, 1, 1)));

    FSlateVectorImageBrush* VectorImageBrush1 = new FSlateVectorImageBrush(NodeIconPath, NodeMargin);
    return SNew(SImage)
		//.FlowDirectionPreference(EFlowDirectionPreference::RightToLeft)
		//.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,-60.0f)))
		//.FlipForRightToLeftFlowDirection(true)
        .Image(VectorImageBrush1);
};


void SPMSEdGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	TopNodeBox.Reset();
	BottomNodeBox.Reset();
	//USlateVectorArtData InMeshData;
	//InMeshData.

	//FSlateFontInfo Font = FSlateFontInfo();
	int32 FontSize = 30;
	FSlateFontInfo FontDefault = FCoreStyle::GetDefaultFontStyle("Regular", FontSize);
	//FSlateBrush CheckBox =  new BOX_BRUSH( "/Persona/StateMachineEditor/StateNode_Node_Body", FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f) )
	FSlateBoxBrush* StateNodeCheckBox = new FSlateBoxBrush(FPaths::EngineContentDir() / TEXT("Editor/Slate")/"/Persona/StateMachineEditor/StateNode_Node_Body.png", FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	FSlateBoxBrush* StateNodeCheckBox2 = new FSlateBoxBrush(FPaths::EngineContentDir() / TEXT("Editor/Slate")/"/Persona/StateMachineEditor/StateNode_Node_Button_Left.png", FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	//FSlateBoxBrush* StateNodeCheckBox = new FSlateBoxBrush(FPaths::ProjectPluginsDir() / TEXT("PMS/Resources/NodeBrush/StateNode_Node_Body.png"), FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	FSlateBoxBrush* LeftCheckBox = new FSlateBoxBrush(FPaths::ProjectPluginsDir() / TEXT("PMS/Resources/NodeBrush/StateNode_Node_Button_Left.png"), FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	FSlateBoxBrush* RightCheckBox = new FSlateBoxBrush(FPaths::ProjectPluginsDir() / TEXT("PMS/Resources/NodeBrush/StateNode_Node_Button_Right.png"), FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	FSlateBoxBrush* CenterCheckBox = new FSlateBoxBrush(FPaths::ProjectPluginsDir() / TEXT("PMS/Resources/NodeBrush/StateNode_Node_Button_Center.png"), FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	
	//FSlateFontInfo font = FSlateFontInfo()
	//SNew(STextBlock);
	//Font.Size = 100;
	FString NodeNameFString = IconName->LeftChop(4);
	FName NodeName = FName("PMSEditor.NodeIcons."+NodeNameFString);
	FVector2D PinHorizontalBoxSize = FVector2D(NodeSize->X*0.6,NodeSize->Y*0.25);

	FString NodeShape = FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/NodeShapes/bone.json");
	TArray<TArray<FClipSMTriangle>> Shapes = StaticMeshFromJson(NodeShape);
	
	GetOrAddSlot(ENodeZone::Center)
	//TODO 可否通过设置SlotSize来限制节点大小，确认为什么会有边框与节点大小不匹配的问题，如何让外边界clipping内部checkbox
	//.HAlign(HAlign_Fill)
	.SlotSize(*NodeSize)
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		//.BorderBackgroundColor(*NodeColor-FLinearColor(0.5f,0.5f,0.5f,0.0f))
		.BorderBackgroundColor(*NodeColor)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(FMargin(0.0f,0.0f))
		.Clipping(EWidgetClipping::ClipToBoundsAlways)
		[
			SNew(SHorizontalBox)	                
            + SHorizontalBox::Slot()
            //.FillWidth(0.145f)
            .AutoWidth()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			.Padding(FMargin(0.0f,0.0f))
            [
                SNew(SCheckBox)
                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Bypass"))
				//.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBoxAlt"))
    //             //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
    //             .CheckedImage(FPMSEditorStyle::Get().GetBrush("PMSEditor.NodeFlagBrushes.Left"))
    //             //.CheckedImage(new FSlateColorBrush(FLinearColor(300.f,.65f,1.f).HSVToLinearRGB()))
    //             .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
    //             .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
    //             .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
    //             .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
				// .Clipping(EWidgetClipping::ClipToBoundsAlways)
    //             .Padding(FMargin(14.0f,29.0f))
                
			]
#if insert
			INSERT_BLOCK
#endif
            + SHorizontalBox::Slot()
            //.FillWidth(0.145f)
            .AutoWidth()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			.Padding(FMargin(0.0f,0.0f))
            [
                SNew(SCheckBox)
                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Lock"))
                // //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                // //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                // .CheckedImage(FPMSEditorStyle::Get().GetBrush("PMSEditor.NodeFlagBrushes.Right"))
                // .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                // .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                // .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                // .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                // .Clipping(EWidgetClipping::ClipToBoundsAlways)
                // .Padding(FMargin(14.0f,29.0f))
            ]
#if insert
			INSERT_BLOCK
#endif
			+ SHorizontalBox::Slot()
			.AutoWidth()
			//.FillWidth(0.4f)
            .VAlign(VAlign_Center)
            .HAlign(HAlign_Center)
			.Padding(FMargin(*NodePadding))
			//.Padding(FMargin(0.5f,0.5f))
			[
				//NodeIcon(*IconName,*NodeMargin)
				SNew(SImage)
				.Image(FPMSEditorStyle::Get().GetBrush(NodeName))
				.DesiredSizeOverride(FVector2D(48.0f,48.0f))
			]
#if insert
			INSERT_BLOCK
#endif
            + SHorizontalBox::Slot()
            //.FillWidth(0.145f)
            .AutoWidth()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			.Padding(FMargin(0.0f,0.0f))
            [
                SNew(SCheckBox)
                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Template"))
    //             //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
    //             //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
    //             .CheckedImage(StateNodeCheckBox2)
    //             .CheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .5f, 1.f).HSVToLinearRGB()))
    //             .CheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .5f, .65f).HSVToLinearRGB()))
    //             .UncheckedHoveredImage(new FSlateColorBrush(FLinearColor(300.f, .3f, 1.f).HSVToLinearRGB()))
    //             .UncheckedPressedImage(new FSlateColorBrush(FLinearColor(300.f, .3f, .7f).HSVToLinearRGB()))
				// .Clipping(EWidgetClipping::ClipToBoundsAlways)
    //             .Padding(FMargin(14.0f,29.0f))
			]
#if insert
			INSERT_BLOCK
#endif
            + SHorizontalBox::Slot()
            //.FillWidth(0.145f)
            .AutoWidth()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
			.Padding(FMargin(0.0f,0.0f))
            [
                SNew(SCheckBox)
                .Style(&FPMSEditorStyle::Get().GetWidgetStyle<FCheckBoxStyle>("PMSEditor.NodeFlags.Display"))
                // //.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
                // //.CheckedImage(new FSlateRoundedBoxBrush(FAppStyle::Get().GetSlateColor("Colors.AccentPink"),2.f))
                // .CheckedImage(StateNodeCheckBox2)
                // .CheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryHover")))
                // .CheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.PrimaryPress")))
                // .UncheckedHoveredImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                // .UncheckedPressedImage(new FSlateColorBrush(FAppStyle::Get().GetSlateColor("Colors.Hover2")))
                // .Clipping(EWidgetClipping::ClipToBoundsAlways)
                // .Padding(FMargin(14.0f,29.0f))
                //.OnCheckStateChanged(this, )
            ]
		]
		
	];

	/*TODO 确认是否可以将TopNodeBox移动到其他ENodeZone之中
	 *猜测如果是没有被占用的ENodeZone就可以使用，否则得在类里面override掉
	 *可以直接使用SlotOffset来移动Slot
	 *Clip的设置
	*/
	GetOrAddSlot(ENodeZone::Right)
	.SlotSize(PinHorizontalBoxSize)
	.SlotOffset(FVector2D((NodeSize->X-PinHorizontalBoxSize.X)/2,(+NodeSize->Y+5)))
	.HAlign(HAlign_Center)
	//.VAlign(VAlign_Center)
	[
		SAssignNew(BottomNodeBox, SHorizontalBox)
		//.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,20.0f)))		
	];
	GetOrAddSlot(ENodeZone::Left)
	.SlotSize(PinHorizontalBoxSize)
	.SlotOffset(FVector2D((NodeSize->X-PinHorizontalBoxSize.X)/2,-(PinHorizontalBoxSize.Y+5)))
	.HAlign(HAlign_Center)
	//.VAlign(VAlign_Top)
	[
		SAssignNew(TopNodeBox, SHorizontalBox)
		//.RenderTransform(FSlateRenderTransform(FVector2D(0.0f,-20.0f)))
	];
	GetOrAddSlot(ENodeZone::BottomCenter)
	//.SlotSize(FVector2D(300.f,NodeSize->Y))
	.SlotOffset(FVector2D(NodeSize->X,(NodeSize->Y-FontSize*3.0f/2.0f)/2))
	//.HAlign(HAlign_Fill)
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SAssignNew(Label,SEditableText)
			.Text(FText::FromString("TestNodeName"))
			.Clipping(EWidgetClipping::Inherit)
			.Font(FontDefault)
			.ColorAndOpacity(FLinearColor(1.0f,1.0f,1.0f,0.8))
			//.TextShapingMethod(ETextShapingMethod::FullShaping)
			//.TextFlowDirection(ETextFlowDirection::RightToLeft)
			//.MinDesiredWidth(100000.0f)
			//.Font(Font)
		]
	];
	GetOrAddSlot(ENodeZone::BottomRight)
	.SlotOffset(*NodeSize)
	.SlotSize(*NodeSize)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(S2DMeshWidget)
			.MeshData(Shapes[0])
		]
	];
	/*GetOrAddSlot(ENodeZone::Center)
	[

		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
		.Padding(FMargin(*NodeMargin))
		.BorderBackgroundColor(*NodeColor)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SAssignNew(TopNodeBox, SHorizontalBox)
			]
			+ SVerticalBox::Slot()
			//.AutoHeight()
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
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
                #1#
                +SHorizontalBox::Slot()
                .VAlign(VAlign_Fill)
                .HAlign(HAlign_Fill)
                //.AutoWidth()
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
                .VAlign(VAlign_Fill)
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
                .VAlign(VAlign_Center)
                
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
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SAssignNew(BottomNodeBox, SHorizontalBox)
			]
		]
	];*/
	
	CreatePinWidgets();
}
void SPMSEdGraphNode::SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel)
{
	check( !OwnerGraphPanelPtr.IsValid() );
	SetParentPanel(OwnerPanel);
	OwnerGraphPanelPtr = OwnerPanel;
	GraphNode->DEPRECATED_NodeWidget = SharedThis(this);

	//TODO: I am not sure this variable should be named like this
	/*Once we have an owner, and if hide Unused pins is enabled, we need to remake our pins to drop the hidden ones*/
	if(OwnerGraphPanelPtr.Pin()->GetPinVisibility() != SGraphEditor::Pin_Show 
		&& TopNodeBox.IsValid()
		&& BottomNodeBox.IsValid())
	{
		this->TopNodeBox->ClearChildren();
		this->BottomNodeBox->ClearChildren();
		CreatePinWidgets();
	}
}
void SPMSEdGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	//SGraphNode::AddPin(PinToAdd);
	
	//TODO This is for Horizontal Layout for Houdini Style
	PinToAdd->SetOwner(SharedThis(this));
	PinToAdd->SetColorAndOpacity(FLinearColor(0.9f,0.9f,0.9f,1.0f));
	PinToAdd->SetClipping(EWidgetClipping::ClipToBoundsAlways);

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = (PinObj != nullptr) && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}
	//TODO Need to figure out why Settings->GetInputPinPadding() can not be used
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		TopNodeBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(1.0f)
		.Padding(FMargin(1.0f,1.0f))
		[
			PinToAdd
		];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
		{
		BottomNodeBox->AddSlot()
		.AutoWidth()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(1.0f)
		.Padding(FMargin(1.0f,1.0f))
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
		}
}

void SPMSEdGraphNode::CreatePinWidgets()
{
	UPMSEdGraphNode* PMSEdGraphNode = CastChecked<UPMSEdGraphNode>(GraphNode);
	for(auto Pin:PMSEdGraphNode->Pins)
	{
		if(!Pin->bHidden)
		{
			/*TODO SGraphPin 要改为自己实现的子类来实现不一样的外观*/
			TSharedPtr<SGraphPin>NewPin = SNew(SPMSEdGraphPin, Pin);
			/*可否编辑直接继承节点可否编辑*/
			//NewPin->SetIsEditable(IsEditable);
			AddPin(NewPin.ToSharedRef());
		}
	}
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
        	//TSharedPtr<SGraphPanel> ParentPanel = GetOwnerPanel();
        	//TSharedPtr<SNodePanel> NodePanel = ParentPanel;
        	//SharedThis(SNode)
        	//TSharedPtr<SNodePanel> NodePanel = ParentPanelPtr.Pin();
			//NodePanel->NodeUnderMousePtr;
        	
        	//Cast<SNodePanel>(GetParentWidget());
        	//GetOwnerPanel()->NodeUnderMousePtr;
        	//if(ParentPanel->
        	Cast<UPMSEdGraphNode>(GraphNode)->PMSSnapToGrid(128.0f,16.0f);
        	// UPMSEdGraph* Graph = Cast<UPMSEdGraph>(GraphNode->GetOuter());
        	// Graph->SelectNode = Cast<UPMSEdGraphNode>(GraphNode);
            // UpdateGraphNode();
        }
    }
}

