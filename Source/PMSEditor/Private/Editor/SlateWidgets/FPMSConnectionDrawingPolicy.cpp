// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/FPMSConnectionDrawingPolicy.h"


FPMSConnectionDrawingPolicy::FPMSConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,  UEdGraph* InGraphObj)
	:FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,InDrawElements)
	,GraphObj(InGraphObj)
{
	ArrowImage = nullptr;
}

void FPMSConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	const FVector2D StartPoint = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndPoint = FGeometryHelper::CenterOf(EndGeom);

	FConnectionDrawingPolicy::DrawSplineWithArrow(StartPoint, EndPoint, Params);
}

void FPMSConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	const FVector2D Dir = (End -Start).GetSafeNormal();
	const FVector2D Down = FVector2D(0,60.0f*ZoomFactor);
	FSlateDrawElement::MakeCubicBezierSpline(
		DrawElementsList,
		LayerId,
		FPaintGeometry(),
		Start,Start+Down,
		FVector2D(End.X,FMath::Min((Start.Y+Down.Y),(End.Y-Down.Y))),End,
		Params.WireThickness,
		ESlateDrawEffect::None,
		Params.WireColor
		);
	// FSlateDrawElement::MakeDrawSpaceSpline(
	// 	DrawElementsList,
	// 	LayerId,
	// 	Start, Down,
	// 	End, Down,
	// 	Params.WireThickness,
	// 	ESlateDrawEffect::None,
	// 	Params.WireColor
	// 	);
}

void FPMSConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionDrawingPolicy::DrawPreviewConnector(PinGeometry, StartPoint, EndPoint, Pin);
}

