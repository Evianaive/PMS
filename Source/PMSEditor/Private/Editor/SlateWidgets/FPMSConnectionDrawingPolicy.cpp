// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/FPMSConnectionDrawingPolicy.h"

#include <Vulkan/Include/vulkan/vulkan_core.h>


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
	// float Xadd = 0.0f;
	// float Yadd = 0.0f;
	// if(Diff.X>120.0f)
	// {
	// 	Xadd = 30.0f;
	// 	Yadd = FMath::Max((AbsDiff.Y-60.0f)/2.0f,(AbsDiff.X-120.0f)/4.0f);
	// }
	// else
	// {
	// 	Xadd = AbsDiff.X/4.0f;
	// 	Yadd = (AbsDiff.Y)/2.0f;
	// }
	float Xadd = AbsDiff.X/4.0f;
	float Yadd = AbsDiff.Y/2.0f;
	//FMath::Max(FMath::Max(Xadd,30.0f),FMath::Min(Yadd,Xadd));
	
	float radius = FMath::Min(MaxRadius,FMath::Max(FMath::Min(Xadd,30.f*ZoomFactor),Yadd));

	TArray<FVector2D> LineVertexArray;
	// FVector2D NewPos = Start;
	// for(int i=0;i<4;i++)
	// {
	// 	NewPos += FVector2D(5.0f);
	// 	LineVertexArray.Add(NewPos);		
	// }
	// 
	
	//LineVertexArray.Empty();
	//LineVertexArray.Init();
	//TArray<FColor> LineVertexColorArray;
	
	//if(Diff.X>0)
	//{
		LineVertexArray.Add(Start);
		FVector2D EndCircle = End-FVector2D(radius,0.0f);
		FVector2D StartCircle = Start+FVector2D(radius,0.0f);
	
		FVector2D CircleDiff = (EndCircle - StartCircle);
		float CircleDiffLen = FVector2D::Distance(EndCircle,StartCircle);
		
		FVector2D CircleDiffN = CircleDiff.GetSafeNormal(0.01f);
		float Angel = FMath::Acos(-CircleDiffN.X);
	if(Diff.Y<0)
	{
		Angel = 2*PI - Angel;
	}
	Angel -= FMath::Acos(radius*2/CircleDiffLen);
		int RadSegments = int(FMath::RadiansToDegrees(Angel)/4.0f);
		float RadStep = Angel/RadSegments;
		float curRad = 0.0f;
		
		for(int i=1;i<RadSegments;i++)
		{
			curRad += RadStep;
			FVector2D CurVertex = FVector2D(-FMath::Cos(curRad),FMath::Sin(curRad));
			LineVertexArray.Add(CurVertex*radius+StartCircle);
			//LineVertexColorArray.Add(FColor::White);
		}
		curRad = Angel;
		for(int i=1;i<RadSegments;i++)
		{
			curRad -= RadStep;
			FVector2D CurVertex = FVector2D(FMath::Cos(curRad),-FMath::Sin(curRad));
			LineVertexArray.Add(CurVertex*radius+EndCircle);
			//LineVertexColorArray.Add(FColor::White);
		}
	LineVertexArray.Add(End);
		
		//Angle
	//}
	
	FSlateDrawElement::MakeLines(DrawElementsList,LayerId,FPaintGeometry(),LineVertexArray);
	//Todo 直接显示Debug信息
	//FSlateDrawElement::MakeText()
}

void FPMSConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionDrawingPolicy::DrawPreviewConnector(PinGeometry, StartPoint, EndPoint, Pin);
}

