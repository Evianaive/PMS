// Copyright Epic Games, Inc. All Rights Reserved.

#include "Editor/SlateWidgets/S2DMeshWidget.h"
#include "Rendering/DrawElements.h"
//#include "Modules/ModuleManager.h"
//#include "Materials/MaterialInterface.h"
//#include "Materials/MaterialInstanceDynamic.h"
#include "Framework/Application/SlateApplication.h"

//#include "SlateMaterialBrush.h"
#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateColorBrush.h"
#include "Runtime/SlateRHIRenderer/Public/Interfaces/ISlateRHIRendererModule.h"
//#include "Slate/SlateVectorArtData.h"
//#include "Slate/SlateVectorArtInstanceData.h"



/** Populate OutSlateVerts and OutIndexes with data from this static mesh such that Slate can render it. */
static void SlateMeshToSlateRenderData(const TArray<FClipSMTriangle>& DataSource, TArray<FSlateVertex>& OutSlateVerts, TArray<SlateIndex>& OutIndexes)
{
	// Populate Index data
	OutIndexes.Empty();
	OutIndexes.Reserve(DataSource.Num()*3);
	OutSlateVerts.Empty();
	OutSlateVerts.Reserve(DataSource.Num()*3);
	int vtxid=0;
	for(auto trin:DataSource)
	{
		for(auto vtx:trin.Vertices)
		{
			FSlateVertex& NewVert = OutSlateVerts[OutSlateVerts.AddUninitialized()];
			
			// Copy Position
			{	
				NewVert.Position[0] = vtx.Pos[0];
				NewVert.Position[1] = vtx.Pos[2];
			}
			// Copy Color
			{
				NewVert.Color = vtx.Color;
			}
			// Copy all the UVs that we have, and as many as we can fit.
			{
				NewVert.TexCoords[0] = vtx.UVs[0].X;
				NewVert.TexCoords[1] = vtx.UVs[0].Y;
				
				NewVert.TexCoords[2] = vtx.UVs[1].X;
				NewVert.TexCoords[3] = vtx.UVs[1].Y;
				
				NewVert.MaterialTexCoords[0] = vtx.UVs[2].X;
				NewVert.MaterialTexCoords[1] = vtx.UVs[2].Y;
				// NewVert.TexCoords[0] = 0.5f;
				// NewVert.TexCoords[1] = 0.5f;
				//
				// NewVert.TexCoords[2] = 0.5f;
				// NewVert.TexCoords[3] = 0.5f;
				//
				// NewVert.MaterialTexCoords[0] = 0.5f;
				// NewVert.MaterialTexCoords[1] = 0.5f;
			}
			OutIndexes.Add(vtxid);
			vtxid++;
		}
	}
}


void S2DMeshWidget::Construct(const FArguments& Args)
{
	if (Args._MeshData.Num() != 0)
	{
		AddMesh(Args._MeshData);
	}
}

uint32 S2DMeshWidget::AddMesh(const TArray<FClipSMTriangle>& InMeshData)
{	
	FRenderData& NewRenderData = RenderData[RenderData.Add(FRenderData())];
	//NewRenderData.Brush = MakeShareable(new FSlateDynamicImageBrush( FName(TEXT("None")),	FVector2D::ZeroVector));
	NewRenderData.Brush = new FSlateBoxBrush(FPaths::EngineContentDir() / TEXT("Editor/Slate")/"/Persona/StateMachineEditor/StateNode_Node_Body.png", FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f));
	//NewRenderData.Brush = MakeShareable(new FSlateBrush(ESlateBrushDrawType::Image, NAME_None, FMargin(0.0f), ESlateBrushTileType::NoTile, ESlateBrushImageType::NoImage, FVector2D::ZeroVector, FLinearColor(255,255,255)));
	//NewRenderData.Brush = MakeShareable(new FSlateBoxBrush(FPaths::ProjectPluginsDir() / TEXT("PMS/Resources/NodeBrush/StateNode_Node_Button_Center.png"), FMargin(16.f/64.f, 25.f/64.f, 16.f/64.f, 16.f/64.f)));
	//wRenderData.RenderingResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle( *NewRenderData.Brush );
	NewRenderData.RenderingResourceHandle = NewRenderData.Brush->GetRenderingResource();
	
	SlateMeshToSlateRenderData(InMeshData, NewRenderData.VertexData, NewRenderData.IndexData);
	return RenderData.Num()-1;
}

void S2DMeshWidget::ClearRuns(int32 NumRuns)
{
	RenderRuns.Reset(NumRuns);
}

int32 S2DMeshWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	if (RenderRuns.Num() > 0)
	{
		// We have multiple render runs. Assume that we have per-instance data.
		for (int RunIndex = 0; RunIndex < RenderRuns.Num(); ++RunIndex)
		{
			const FRenderRun& Run = RenderRuns[RunIndex];
			const FRenderData& RunRenderData = RenderData[Run.GetMeshIndex()];
			if (RunRenderData.RenderingResourceHandle.IsValid() && RunRenderData.VertexData.Num() > 0 && RunRenderData.IndexData.Num() > 0 && RunRenderData.PerInstanceBuffer.IsValid())
			{
				ensure(Run.GetInstanceOffset() + Run.GetNumInstances() <= RunRenderData.PerInstanceBuffer->GetNumInstances());
				FSlateDrawElement::MakeCustomVerts(OutDrawElements,
					LayerId,
					RunRenderData.RenderingResourceHandle,
					RunRenderData.VertexData,
					RunRenderData.IndexData,
					RunRenderData.PerInstanceBuffer.Get(),
					Run.GetInstanceOffset(),
					Run.GetNumInstances());
			}
			// else
			// {
			// 	if( !GUsingNullRHI )
			// 	{
			// 		UE_LOG(LogTemp, Warning, TEXT("SMeshWidget did not render a run because of one of these Brush: %s, InstanceBuffer: %s, NumVertexes: %d, NumIndexes: %d"),
			// 			RunRenderData.RenderingResourceHandle.IsValid() ? TEXT("valid") : TEXT("nullptr"),
			// 			RunRenderData.PerInstanceBuffer.IsValid() ? TEXT("valid") : TEXT("nullptr"),
			// 			RunRenderData.VertexData.Num(),
			// 			RunRenderData.IndexData.Num());
			// 	}
			// }
		}
	}
	else
	{
		// We have no render runs. Render all the meshes in order they were added
		for (int i = 0; i < RenderData.Num(); ++i)
		{
			const FRenderData& RunRenderData = RenderData[i];
			if (RunRenderData.RenderingResourceHandle.IsValid() && RunRenderData.VertexData.Num() > 0 && RunRenderData.IndexData.Num() > 0)
			{
				if (RunRenderData.PerInstanceBuffer.IsValid())
				{
					// Drawing instanced widgets
					const int32 NumInstances = RunRenderData.PerInstanceBuffer->GetNumInstances();
					if (NumInstances > 0)
					{
						FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, RunRenderData.RenderingResourceHandle, RunRenderData.VertexData, RunRenderData.IndexData, RunRenderData.PerInstanceBuffer.Get(), 0, NumInstances);
					}
				}
				else
				{
					// Drawing a single widget, no instancing
					FSlateDrawElement::MakeCustomVerts(OutDrawElements, LayerId, RunRenderData.RenderingResourceHandle, RunRenderData.VertexData, RunRenderData.IndexData, nullptr, 0, 0);
				}
			}
			// else
			// {
			// 	if( !GUsingNullRHI )
			// 	{
			// 		UE_LOG(LogTemp, Warning, TEXT("SMeshWidget did not render a run because of one of these Brush: %s, NumVertexes: %d, NumIndexes: %d"),
			// 			RunRenderData.RenderingResourceHandle.IsValid() ? TEXT("valid") : TEXT("nullptr"),
			// 			RunRenderData.VertexData.Num(),
			// 			RunRenderData.IndexData.Num());
			// 	}
			// }
		}
	}

	return LayerId;
}


FVector2D S2DMeshWidget::ComputeDesiredSize(float) const
{
	return FVector2D(256,256);
}

