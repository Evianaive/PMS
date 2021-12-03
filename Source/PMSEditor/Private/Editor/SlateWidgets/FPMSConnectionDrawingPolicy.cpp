// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/SlateWidgets/FPMSConnectionDrawingPolicy.h"

#include <Vulkan/Include/vulkan/vulkan_core.h>


FPMSConnectionDrawingPolicy::FPMSConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,	const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements,  UEdGraph* InGraphObj)
	:FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect,InDrawElements)
	,GraphObj(InGraphObj)
{
	//ArrowImage = nullptr;
}
void FPMSConnectionDrawingPolicy::DrawPinGeometries(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	for (TMap<TSharedRef<SWidget>, FArrangedWidget>::TIterator ConnectorIt(InPinGeometries); ConnectorIt; ++ConnectorIt)
	{
		TSharedRef<SWidget> SomePinWidget = ConnectorIt.Key();
		SGraphPin& PinWidget = static_cast<SGraphPin&>(SomePinWidget.Get());
		UEdGraphPin* ThePin = PinWidget.GetPinObj();

		if (ThePin->Direction == EGPD_Output)
		{
			for (int32 LinkIndex=0; LinkIndex < ThePin->LinkedTo.Num(); ++LinkIndex)
			{
				FArrangedWidget* LinkStartWidgetGeometry = nullptr;
				FArrangedWidget* LinkEndWidgetGeometry = nullptr;

				UEdGraphPin* TargetPin = ThePin->LinkedTo[LinkIndex];

				DetermineLinkGeometry(ArrangedNodes, SomePinWidget, ThePin, TargetPin, /*out*/ LinkStartWidgetGeometry, /*out*/ LinkEndWidgetGeometry);

				if (( LinkEndWidgetGeometry && LinkStartWidgetGeometry ) && !IsConnectionCulled( *LinkStartWidgetGeometry, *LinkEndWidgetGeometry ))
				{
					FConnectionParams Params;
					DetermineWiringStyle(ThePin, TargetPin, /*inout*/ Params);
					DrawSplineWithArrow(LinkStartWidgetGeometry->Geometry, LinkEndWidgetGeometry->Geometry, Params);
				}
			}
		}
	}
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
void FPMSConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
	/*Calculate polyline vertices position*/
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

	/*Calculate vertex color along polyline*/
	TArray<FLinearColor> LineVertexColorArray;
	LineVertexColorArray.Init(FLinearColor(1.0f,1.0f,1.0f,0.8f),LineVertexArray.Num());

	/*Detect whether cursor overlaps with connection polyline*/
	FVector2D StartCircleEnd = LineVertexArray[StartRadSegments-1];
	FVector2D EndCircleStart = LineVertexArray[StartRadSegments];
	if (Settings->bTreatSplinesLikePins)
	{
		// Distance to consider as an overlap
		const float QueryDistanceTriggerThresholdSquared = FMath::Square(Settings->SplineHoverTolerance + Params.WireThickness * 0.5f);

		// Distance to pass the bounding box cull test. This is used for the bCloseToSpline output that can be used as a
		// dead zone to avoid mistakes caused by missing a double-click on a connection.
		const float QueryDistanceForCloseSquared = FMath::Square(FMath::Sqrt(QueryDistanceTriggerThresholdSquared) + Settings->SplineCloseTolerance);

		bool bCloseToSpline = false;
		{
			FBox2D Bounds(ForceInit);			
			Bounds += End + Diff.GetSignVector()*radius;
			Bounds += Start - Diff.GetSignVector()*radius;

			bCloseToSpline = Bounds.ComputeSquaredDistanceToPoint(LocalMousePosition) < QueryDistanceForCloseSquared;

			// Draw the bounding box for debugging
#if 0
#define DrawSpaceLine(Point1, Point2, DebugWireColor) {const FVector2D FakeTangent = (Point2 - Point1).GetSafeNormal(); FSlateDrawElement::MakeDrawSpaceSpline(DrawElementsList, LayerId, Point1, FakeTangent, Point2, FakeTangent, ClippingRect, 1.0f, ESlateDrawEffect::None, DebugWireColor); }

			if (bCloseToSpline)
			{
				const FLinearColor BoundsWireColor = bCloseToSpline ? FLinearColor::Green : FLinearColor::White;

				FVector2D TL = Bounds.Min;
				FVector2D BR = Bounds.Max;
				FVector2D TR = FVector2D(Bounds.Max.X, Bounds.Min.Y);
				FVector2D BL = FVector2D(Bounds.Min.X, Bounds.Max.Y);

				DrawSpaceLine(TL, TR, BoundsWireColor);
				DrawSpaceLine(TR, BR, BoundsWireColor);
				DrawSpaceLine(BR, BL, BoundsWireColor);
				DrawSpaceLine(BL, TL, BoundsWireColor);
			}
#endif
		}

		if (bCloseToSpline)
		{
			// Find the closest approach to the spline
			float ClosestDistanceSquared = FLT_MAX;	
			float MidConnectionLength = FVector2D::Distance(StartCircleEnd,EndCircleStart);
			float ConnectionPolyLineLength = radius*(StartAngel + EndAngel) + MidConnectionLength;
			
			FVector2D TempMousePos = bMirrorEnd?FVector2D(2*Start.X - LocalMousePosition.X,LocalMousePosition.Y):LocalMousePosition;
			
			
			bool bClose = false;
			bool bHovered = false;
			FVector2D MouseDiff(ForceInit),MouseDiffN(ForceInit);
			float MouseDiffLen,MouseAngel;
			float SquaredDistToPin1;
			float SquaredDistToPin2;

			/*check if hover on MidConnection*/
			if(!bHovered && !bClose)
			{
				float ProjectLength = FVector2D::DotProduct(LocalMousePosition-StartCircleEnd,EndCircleStart-StartCircleEnd)/MidConnectionLength;
				if(ProjectLength>0 && ProjectLength<MidConnectionLength)
				{
					float MouseLineDistSquared = FVector2D::DistSquared(LocalMousePosition,StartCircleEnd) - FMath::Square(ProjectLength);
					if(MouseLineDistSquared<QueryDistanceTriggerThresholdSquared)
					{
						bHovered = true;
						ClosestDistanceSquared = MouseLineDistSquared;
						SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? ProjectLength + StartAngel*radius : FLT_MAX;
						SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? ConnectionPolyLineLength - ProjectLength - StartAngel*radius : FLT_MAX;
						// bClose = true;
					}
					else if(MouseLineDistSquared<QueryDistanceForCloseSquared)
					{
						bClose = true;
					}
				}
			}
			/*check if hover on StartCircle*/
			if(!bHovered && !bClose){
				MouseDiff = (TempMousePos - StartCircle);
                MouseDiffLen = FVector2D::Distance(TempMousePos,StartCircle);	
                MouseDiffN = CircleDiff.GetSafeNormal(0.01f);
                MouseAngel = FMath::Acos(bMirrorStartCircle?MouseDiffN.X:-MouseDiffN.X);
                if(MouseDiff.Y<0)
                {
                	MouseAngel = 2*PI - MouseAngel;
                }
                if(MouseAngel<StartAngel)
                {
                	if(FMath::Square(MouseDiffLen-radius)<QueryDistanceTriggerThresholdSquared)
                	{
                		bHovered = true;
                		ClosestDistanceSquared = FMath::Square(MouseDiffLen-radius);
                		SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? MouseAngel*radius : FLT_MAX;
                		SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? ConnectionPolyLineLength - MouseAngel*radius : FLT_MAX;
                		// bClose = true;
                	}
                	else if(FMath::Square(MouseDiffLen-radius)<QueryDistanceForCloseSquared)
                	{
                		bClose = true;
                	}
                }
			}
			/*check if hover on EndCircle*/
			if(!bHovered && !bClose){
				MouseDiff = (TempMousePos - EndCircle);
				MouseDiffLen = FVector2D::Distance(TempMousePos,EndCircle);	
				MouseDiffN = CircleDiff.GetSafeNormal(0.01f);
				MouseAngel = FMath::Acos(MouseDiffN.X);
				if(MouseDiff.Y>0)
				{
					MouseAngel = 2*PI - MouseAngel;
				}
				if(MouseAngel<EndAngel)
				{
					if(FMath::Abs(MouseDiffLen-radius)<QueryDistanceTriggerThresholdSquared)
					{
						bHovered = true;
                		ClosestDistanceSquared = FMath::Square(MouseDiffLen-radius);
						SquaredDistToPin1 = (Params.AssociatedPin1 != nullptr) ? ConnectionPolyLineLength - MouseAngel*radius : FLT_MAX;
						SquaredDistToPin2 = (Params.AssociatedPin2 != nullptr) ? MouseAngel*radius : FLT_MAX;

						// bClose = true;
					}
					else if(FMath::Square(MouseDiffLen-radius)<QueryDistanceForCloseSquared)
					{
						bClose = true;
					}
				}
			}
			/*Set OverlapResult*/
			if (bHovered)
			{
				if (ClosestDistanceSquared < SplineOverlapResult.GetDistanceSquared())
				{
					SplineOverlapResult = FGraphSplineOverlapResult(Params.AssociatedPin1, Params.AssociatedPin2, ClosestDistanceSquared, SquaredDistToPin1, SquaredDistToPin2, true);
				}
			}
			else if (bClose)
			{
				SplineOverlapResult.SetCloseToSpline(true);
			}
#if 0		
			/* Debug Variable */
			FVector2D ArrowPoint = (StartCircleEnd + EndCircleStart)/2 - ArrowRadius;		
			FString HoveredPinsNum = TEXT("HoveredPinsNum = ") + FString::FromInt(HoveredPins.Num())
				+ TEXT("\nStartAngel = ") + FString::SanitizeFloat(FMath::RadiansToDegrees(StartAngel))
				+ TEXT("\nEndAngel = ") + FString::SanitizeFloat(FMath::RadiansToDegrees(EndAngel))
				+ TEXT("\nbClose = ") + (bClose?TEXT("true"):TEXT("flase"))
				+ TEXT("\nbHovered = ") + (bHovered?TEXT("true"):TEXT("flase"))
				+ TEXT("\nClosestDistanceSquared = ") + FString::SanitizeFloat(ClosestDistanceSquared)
				+ TEXT("\nQueryDistanceTriggerThresholdSquared = ") + FString::SanitizeFloat(QueryDistanceTriggerThresholdSquared)
				+ TEXT("\nQueryDistanceForCloseSquared = ") + FString::SanitizeFloat(QueryDistanceForCloseSquared);
			;
	
			FSlateDrawElement::MakeText(
				DrawElementsList,
				ArrowLayerID,
				FPaintGeometry(ArrowPoint, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
				HoveredPinsNum,
				0,HoveredPinsNum.Len(),
				FCoreStyle::GetDefaultFontStyle("Regular", 20)
				);
#endif
		}
	}
	/*Make connection*/
	FSlateDrawElement::MakeLines(DrawElementsList,LayerId,FPaintGeometry(),LineVertexArray,LineVertexColorArray,ESlateDrawEffect::None,FLinearColor::White,true,Params.WireThickness);
	/*Make Arrow if can and needed*/
	if (ArrowImage != nullptr && Diff.Y<0 )
	{
		FVector2D ArrowPoint = (StartCircleEnd + EndCircleStart)/2 - ArrowRadius;
		FSlateDrawElement::MakeRotatedBox(
			DrawElementsList,
			LayerId,
			FPaintGeometry(ArrowPoint, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
			ArrowImage,
			ESlateDrawEffect::None,
			(bMirrorEnd==bMirrorStartCircle)?PI/2 - StartAngel:PI/2 + StartAngel);
	}
}

void FPMSConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionDrawingPolicy::DrawPreviewConnector(PinGeometry, StartPoint, EndPoint, Pin);
}

void FPMSConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, /*inout*/ FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, /*inout*/ Params.WireThickness, /*inout*/ Params.WireColor);
	}
}
