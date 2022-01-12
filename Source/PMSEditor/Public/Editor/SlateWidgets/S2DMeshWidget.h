// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Textures/SlateShaderResource.h"
#include <string>

#include "Rendering/RenderingCommon.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"
#include "GeomTools.h"

class FPaintArgs;
class FSlateWindowElementList;
struct FSlateBrush;

//FClipSMTriangle DefaultTriangle(0);
inline TArray<FClipSMTriangle> DefaultMeshData;
/**
 * A widget that draws vertexes provided by a 2.5D StaticMesh.
 * The Mesh's material is used.
 * Hardware instancing is supported.
 */
class S2DMeshWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(S2DMeshWidget)
		:_MeshData(DefaultMeshData)
	{}
		/** The StaticMesh asset that should be drawn. */
		SLATE_ARGUMENT(TArray<FClipSMTriangle>, MeshData)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

	/**
	 * Draw the InStaticMesh when this widget paints.
	 *
	 * @return the Index of the mesh data that was added; cache this value for use with @see FRenderRun.
	 */
	uint32 AddMesh(const TArray<FClipSMTriangle>& InMeshData);

	/** Discard any previous runs and reserve space for new render runs if needed. */
	void ClearRuns(int32 NumRuns);

	/**
	 * Tell the widget to draw instances of a mesh a given number of times starting at
	 * a given offset.
	 *
	 * @param InMeshIndex        Which mesh to draw; returned by @see AddMesh
	 * @param InInstanceOffset   Start drawing with this instance
	 * @param InNumInstances     Draw this many instances
	 */
	FORCEINLINE void AddRenderRun(uint32 InMeshIndex, uint32 InInstanceOffset, uint32 InNumInstances)
	{
		RenderRuns.Add(FRenderRun(InMeshIndex, InInstanceOffset, InNumInstances));
	}

protected:
	// BEGIN SLeafWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	// END SLeafWidget interface

protected:
	struct FRenderData
	{
		/** Holds a copy of the Static Mesh's data converted to a format that Slate understands. */
		TArray<FSlateVertex> VertexData;
		/** Connectivity data: Order in which the vertexes occur to make up a series of triangles. */
		TArray<SlateIndex> IndexData;
		/** Holds on to the material that is found on the StaticMesh. */
		FSlateBrush* Brush;
		/** A rendering handle used to quickly access the rendering data for the slate element*/
		FSlateResourceHandle RenderingResourceHandle;
		/** Per instance data that can be passed to */
		TSharedPtr<ISlateUpdatableInstanceBuffer> PerInstanceBuffer;
	};
	TArray<FRenderData, TInlineAllocator<3>> RenderData;

private:
	/** Which mesh to draw, starting with which instance offset and how many instances to draw in this run/batch. */
	class FRenderRun
	{
	public:
		FRenderRun(uint32 InMeshIndex, uint32 InInstanceOffset, uint32 InNumInstances)
			: MeshIndex(InMeshIndex)
			, InstanceOffset(InInstanceOffset)
			, NumInstances(InNumInstances)
		{
		}

		uint32 GetMeshIndex() const { return MeshIndex; }
		uint32 GetInstanceOffset() const { return InstanceOffset; }
		uint32 GetNumInstances() const { return NumInstances; }

	private:
		uint32 MeshIndex;
		uint32 InstanceOffset;
		uint32 NumInstances;
	};
	TArray<FRenderRun> RenderRuns;
};


class STestLeafWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(STestLeafWidget)
		:_ShowText(FString("Test"))
	{}
		/** The StaticMesh asset that should be drawn. */
		SLATE_ARGUMENT(FString, ShowText)
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

	FString ShowText;
};



struct FOnPaintHandlerParams
{
	const FGeometry& Geometry;
	const FSlateRect& ClippingRect;
	FSlateWindowElementList& OutDrawElements;
	const int32 Layer;
	const bool bEnabled;

	FOnPaintHandlerParams( const FGeometry& InGeometry, const FSlateRect& InClippingRect, FSlateWindowElementList& InOutDrawElements, int32 InLayer, bool bInEnabled )
		: Geometry( InGeometry )
		, ClippingRect( InClippingRect )
		, OutDrawElements( InOutDrawElements )
		, Layer( InLayer )
		, bEnabled( bInEnabled )
	{
	}

};

/** Delegate type for allowing custom OnPaint handlers */
DECLARE_DELEGATE_RetVal_OneParam( 
	int32,
	FOnPaintHandler,
	const FOnPaintHandlerParams& );

/** Widget with a handler for OnPaint; convenient for testing various DrawPrimitives. */
class SCustomPaintWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SCustomPaintWidget )
		: _OnPaintHandler()
	{}

	SLATE_EVENT( FOnPaintHandler, OnPaintHandler )
SLATE_END_ARGS()

/**
 * Construct this widget
 *
 * @param	InArgs	The declaration data for this widget
 */
void Construct(const FArguments& InArgs)
	{
		OnPaintHandler = InArgs._OnPaintHandler;
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		return FVector2D(128, 128);
	}

	virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override
	{
		if( OnPaintHandler.IsBound() )
		{
			FOnPaintHandlerParams Params( AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, bParentEnabled && IsEnabled() ); 
			OnPaintHandler.Execute( Params );
		}
		else
		{
			FSlateDrawElement::MakeDebugQuad(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry()
			);
		}

		return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled && IsEnabled() );
	}

private:
	FOnPaintHandler OnPaintHandler;
};

class SElementTesting : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SElementTesting ){}
	SLATE_END_ARGS()

	/**
	 * Construct the widget
	 *
	 * @param InArgs   Declartion from which to construct the widget
	 */
	void Construct(const FArguments& InArgs)
	{
		FontScale = 1.0f;

		// Arrange a bunch of DrawElement tester widgets in a vertical stack.
		// Use custom OnPaint handlers.
		this->ChildSlot
		[
			SAssignNew( VerticalBox, SVerticalBox )
			+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				SNew(SCustomPaintWidget)
				.OnPaintHandler(this, &SElementTesting::TestCustomVerts)
			]
		];
	}

	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override
	{
		CenterRotation += InDeltaTime*.3;
		if( CenterRotation > 2*PI)
		{
			CenterRotation-= 2*PI;
		}

		OuterRotation += (InDeltaTime *1.5);
		if( OuterRotation > 2*PI)
		{
			OuterRotation-= 2*PI;
		}
	}

	SElementTesting()
	{
		CenterRotation = 0;
		OuterRotation = 0;
	}

private:
	TSharedPtr<SVerticalBox> VerticalBox;
	float FontScale;
	float CenterRotation;
	float OuterRotation;

	int32 TestCustomVerts(const FOnPaintHandlerParams& InParams)
	{
		// const float Radius = FMath::Min(InParams.Geometry.GetLocalSize().X, InParams.Geometry.GetLocalSize().Y) * 0.5f;
		const FVector2D Center = InParams.Geometry.AbsolutePosition + InParams.Geometry.GetLocalSize() * 0.5f;
		
		const float Radius = 50;
		// const FVector2D Center = FVector2D::ZeroVector;
		
		const FSlateBrush* MyBrush = FCoreStyle::Get().GetBrush("ColorWheel.HueValueCircle");
		// const FSlateBrush* MyBrush = FEditorStyle::GetBrush("Graph.StateNode.Body");

		FSlateResourceHandle Handle = MyBrush->GetRenderingResource();
		const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();

		FVector2D UVCenter = FVector2D::ZeroVector;
		FVector2D UVRadius = FVector2D(1,1);
		if (ResourceProxy != nullptr)
		{
			UVRadius = 0.5f * ResourceProxy->SizeUV;
			UVCenter = ResourceProxy->StartUV + UVRadius;
		}

		// Make a triangle fan in the area allotted
		const int NumTris = 12;
		TArray<FSlateVertex> Verts;
		Verts.Reserve(NumTris*3);

		// Center Vertex
		Verts.AddZeroed();
		{
			FSlateVertex& NewVert = Verts.Last();
			NewVert.Position[0] = Center.X;
			NewVert.Position[1] = Center.Y;
			NewVert.TexCoords[0] = UVCenter.X;
			NewVert.TexCoords[1] = UVCenter.Y;
			// NewVert.TexCoords[2] = UVCenter.X;
			// NewVert.TexCoords[3] = UVCenter.Y;
			NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
			NewVert.Color = FColor::White;
		}

		for (int i = 0; i < NumTris; ++i)
		{
			Verts.AddZeroed();
			{
				const float Angle = (2*PI*i) / NumTris;
				const FVector2D EdgeDirection(FMath::Cos(Angle), FMath::Sin(Angle));
				const FVector2D Edge(Radius*EdgeDirection);
				FSlateVertex& NewVert = Verts.Last();
				NewVert.Position[0] = Center.X + Edge.X;
				NewVert.Position[1] = Center.Y + Edge.Y;
				// NewVert.TexCoords[0] = UVCenter.X + UVRadius.X*EdgeDirection.X;
				// NewVert.TexCoords[1] = UVCenter.Y + UVRadius.Y*EdgeDirection.Y;
				// NewVert.TexCoords[0] = UVCenter.X;
				// NewVert.TexCoords[1] = UVCenter.Y;
				// NewVert.TexCoords[0] = 1.0;
				// NewVert.TexCoords[1] = 1.0;
				NewVert.TexCoords[0] = NewVert.Position[0];
				NewVert.TexCoords[1] = NewVert.Position[1];
				// NewVert.TexCoords[0] = UVCenter.X + UVRadius.X*EdgeDirection.X*2;
				// NewVert.TexCoords[1] = UVCenter.Y + UVRadius.Y*EdgeDirection.Y*2;
				NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;
				NewVert.Color = FColor::White;
			}
		}

		TArray<SlateIndex> Indexes;
		for (int i = 1; i <= NumTris; ++i)
		{
			Indexes.Add(0);
			Indexes.Add(i);
			Indexes.Add( (i+1 > 12) ? (1) : (i+1) );
		}
		// SlateIndex last = Indexes[Indexes.Num()-1];
		// Indexes.Pop();
		// Indexes.Add(last);
		// UE_LOG(LogTemp,Log,TEXT("ff %s"),ToCStr(FVector2D(Indexes[0],Indexes[1]).ToString()));

		FSlateDrawElement::MakeCustomVerts(InParams.OutDrawElements, InParams.Layer, Handle, Verts, Indexes, nullptr, 0, 0);

		return InParams.Layer;
	}
};
