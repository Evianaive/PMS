// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/FPMSConnectionDrawingPolicy.h"

#include <Vulkan/Include/vulkan/vulkan_core.h>


FPMSConnectionDrawingPolicy::FPMSConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,  UEdGraph* InGraphObj)
	:FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,InDrawElements)
	,GraphObj(InGraphObj)
{
	//ArrowImage = nullptr;
}

void FPMSConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	const FVector2D StartPoint = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndPoint = FGeometryHelper::CenterOf(EndGeom);

	FPMSConnectionDrawingPolicy::DrawSplineWithArrow(StartPoint, EndPoint, Params);
}
void FPMSConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params)
{
	// Draw the spline
	DrawConnection(
		WireLayerID,
		StartPoint,
		EndPoint,
		Params);
}
	PRAGMA_DISABLE_OPTIMIZATION
void FPMSConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	// const FVector2D Dir = (End -Start).GetSafeNormal();
	// const FVector2D Down = FVector2D(0,60.0f*ZoomFactor);
	
	// FSlateDrawElement::MakeCubicBezierSpline(
	// 	DrawElementsList,
	// 	LayerId,
	// 	FPaintGeometry(),
	// 	Start,Start+Down,
	// 	FVector2D(End.X,FMath::Min((Start.Y+Down.Y),(End.Y-Down.Y))),End,
	// 	Params.WireThickness,
	// 	ESlateDrawEffect::None,
	// 	Params.WireColor
	// 	);
	
	// FSlateDrawElement::MakeDrawSpaceSpline(
	// 	DrawElementsList,
	// 	LayerId,
	// 	Start, Down,
	// 	End, Down,
	// 	Params.WireThickness,
	// 	ESlateDrawEffect::None,
	// 	Params.WireColor
	// 	);
	
	float MaxRadius = 60.0f*ZoomFactor;
	FVector2D Diff = End - Start;
	FVector2D AbsDiff(FMath::Abs(Diff.X),FMath::Abs(Diff.Y));
	float Xadd = AbsDiff.X/4.0f;
	float Yadd = AbsDiff.Y/2.0f;
	
	float radius = FMath::Min(MaxRadius,FMath::Max(FMath::Min(Xadd,30.f*ZoomFactor),Yadd));
	
	bool bMirrorStartCircle = Diff.Y<0 && radius>AbsDiff.X/2;
	bool bMirrorEnd = Diff.X<0;
	FVector2D TempEnd = bMirrorEnd?FVector2D(End.X-Diff.X*2,End.Y):End; 
	TArray<FVector2D> LineVertexArray;
	
	FVector2D CircleOffset(radius,0.0f); 
	FVector2D EndCircle = TempEnd-CircleOffset;
	FVector2D StartCircle = Start+(bMirrorStartCircle?-1:1) *CircleOffset;

	FVector2D CircleDiff = (EndCircle - StartCircle);
	float CircleDiffLen = FVector2D::Distance(EndCircle,StartCircle);
	
	FVector2D CircleDiffN = CircleDiff.GetSafeNormal(0.01f);
	float Angel = FMath::Acos(-CircleDiffN.X);
	if(Diff.Y<0)
	{
		Angel = 2*PI - Angel;
	}
	Angel -= bMirrorStartCircle?PI/2.0f:FMath::Acos(radius*2/CircleDiffLen);
	float StartAngel = bMirrorStartCircle? 2*PI - Angel : Angel;
	float EndAngel = Angel;
	
	LineVertexArray.Add(Start);
	int StartRadSegments = int(FMath::RadiansToDegrees(StartAngel)/4.0f)+1;
	float StartRadStep = StartAngel/StartRadSegments;
	float curRad = 0.0f;		
	for(int i=1;i<StartRadSegments;i++)
	{
		curRad += StartRadStep;
		FVector2D CurVertex = FVector2D(bMirrorStartCircle?FMath::Cos(curRad):-FMath::Cos(curRad),FMath::Sin(curRad))*radius+StartCircle;
		if(bMirrorEnd)
		{			
			CurVertex.X = 2*Start.X - CurVertex.X;
		}
		LineVertexArray.Add(CurVertex);
	}
	
	int EndRadSegments = int(FMath::RadiansToDegrees(EndAngel)/4.0f)+1;
	float EndRadStep = EndAngel/EndRadSegments;	
	curRad = EndAngel;
	for(int i=1;i<EndRadSegments;i++)
	{
		curRad -= EndRadStep;
		FVector2D CurVertex = FVector2D(FMath::Cos(curRad),-FMath::Sin(curRad))*radius+EndCircle;
		if(bMirrorEnd)
		{
			CurVertex.X = 2*Start.X - CurVertex.X;
		}
		LineVertexArray.Add(CurVertex);
	}
	LineVertexArray.Add(End);

	TArray<FLinearColor> LineVertexColorArray;
	LineVertexColorArray.Init(FLinearColor(1.0f,1.0f,1.0f,0.8f),LineVertexArray.Num());	
	FSlateDrawElement::MakeLines(DrawElementsList,LayerId,FPaintGeometry(),LineVertexArray,LineVertexColorArray);
	//Todo 直接显示Debug信息
	//FSlateDrawElement::MakeText()

	
	if (ArrowImage != nullptr && Diff.Y<0 )
	{
		FVector2D ArrowPoint = (LineVertexArray[StartRadSegments-1] + LineVertexArray[StartRadSegments])/2 - ArrowRadius;
		FSlateDrawElement::MakeRotatedBox(
			DrawElementsList,
			LayerId,
			FPaintGeometry(ArrowPoint, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
			ArrowImage,
			ESlateDrawEffect::None,
			(bMirrorEnd==bMirrorStartCircle)?PI/2 - StartAngel:PI/2 + StartAngel);
	}
}
	PRAGMA_ENABLE_OPTIMIZATION

void FPMSConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionDrawingPolicy::DrawPreviewConnector(PinGeometry, StartPoint, EndPoint, Pin);
}

