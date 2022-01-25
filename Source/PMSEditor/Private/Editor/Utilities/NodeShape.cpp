// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/Utilities/FNodeShape.h"
#include "Json.h"
#include "GeomTools.h"
#include "Misc/Paths.h"

TSharedPtr<FNodeShapeCollection> FNodeShapeCollection::Instance = nullptr;
bool FNodeShapeCollection::bInitialized = false;

FNodeShape::FNodeShape()
{
	Scale = 100;
	FNodeShape(FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/NodeShapes/rect.json"));
}

FNodeShape::FNodeShape(FString FilePath)
{
	FString JsonStr;
	Scale = 100;
	
	bool bLoadSuccess = FFileHelper::LoadFileToString(JsonStr,*FilePath);
	if(!bLoadSuccess)
	{
		
	}
	else
	{		
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		
		FJsonSerializer::Deserialize(JsonReader,JsonObject);
		//variable name is the same as its name in json
		NodeShapeName = FName(JsonObject->GetStringField(FString("name")));
		TSharedPtr<FJsonObject> flags = JsonObject->GetObjectField(FString("flags"));
		TArray<TSharedPtr<FJsonValue>> outline = JsonObject->GetArrayField("outline");
		//TArray<UStaticMesh> ShapeVectors;
		for(int i=0;i<4;i++)
		{
			TArray<TSharedPtr<FJsonValue>> CurOutline = flags->GetObjectField(FString::FromInt(i))->GetArrayField("outline");
			
			// FSlateVertexArray& OutSlateVertices = FlagsVertices[FlagsVertices.AddUninitialized()];
			// FSlateIndexArray& OutSlateIndexes = FlagsIndexes[FlagsIndexes.AddUninitialized()];
			// TArray<FVector2D>& OutSlateLine = FlagsLine[FlagsLine.AddUninitialized()];
			
			FSlateVertexArray OutSlateVertices;
			FSlateIndexArray OutSlateIndexes;
			TArray<FVector2D> OutSlateLine;
			UpdataByJsonValue(OutSlateLine,OutSlateVertices,OutSlateIndexes,CurOutline);
			FlagsVertices.Add(OutSlateVertices);
			FlagsIndexes.Add(OutSlateIndexes);
			FlagsLine.Add(OutSlateLine);
		}
		UpdataByJsonValue(OutLine,OutLineVertices,OutlineIndex,outline);
		
		TArray<TSharedPtr<FJsonValue>> inputs = JsonObject->GetArrayField("inputs");
		TArray<TSharedPtr<FJsonValue>> outputs = JsonObject->GetArrayField("outputs");
		TArray<TSharedPtr<FJsonValue>> icon = JsonObject->GetArrayField("icon");
	}
	
}

FNodeShape::~FNodeShape()
{
	
}

void FNodeShape::UpdataByJsonValue(TArray<FVector2D>& ShapeLine, FSlateVertexArray& ShapeVertices, FSlateIndexArray& ShapeIndexes, TArray<TSharedPtr<FJsonValue>> JsonArray)
{
	//FRawMesh FlagRawMesh;
	// FString MeshName = "Flag"+FString::FromInt(i);
	//init FlagPolygon, param means the count of triangle?
	FClipSMPolygon FlagPolygon(JsonArray.Num()-2);
	
	//FlagRawMesh.VertexPositions.Add()
	//FlagRawMesh.WedgeIndices.Add();
	
	// FVector2D MaxBound(JsonArray[0]->AsArray()[0]->AsNumber()*Scale,JsonArray[1]->AsArray()[0]->AsNumber()*Scale);
	// FVector2D MinBound(MaxBound);
	ShapeLine.Empty();
	ShapeVertices.Empty();
	ShapeIndexes.Empty();
	
	for(TSharedPtr<FJsonValue> Point:JsonArray)
	{
		auto PointElementsArray = Point->AsArray();
		FClipSMVertex Flagvertex;
		Flagvertex.Pos = FVector3f(PointElementsArray[0]->AsNumber()*Scale,0,- PointElementsArray[1]->AsNumber()*Scale);
		ShapeLine.Add(FVector2D(PointElementsArray[0]->AsNumber()*Scale,- PointElementsArray[1]->AsNumber()*Scale));
		Flagvertex.Color = FColor::White;
		FlagPolygon.Vertices.Add(Flagvertex);
		// MaxBound.X = FMath::Max(Flagvertex.Pos.X,MaxBound.X);
		// MaxBound.Y = FMath::Max(Flagvertex.Pos.Z,MaxBound.Y);
		// MinBound.X = FMath::Min(Flagvertex.Pos.X,MinBound.X);
		// MinBound.Y = FMath::Min(Flagvertex.Pos.Z,MinBound.Y);
	}
	// auto PointElementsArray = JsonArray[0]->AsArray();
	// ShapeLine.Add(FVector2D(PointElementsArray[0]->AsNumber()*Scale,PointElementsArray[1]->AsNumber()*Scale));
	//

	//these triangles has unique points
	// TArray<FClipSMTriangle> FlagTriangles;
	// FGeomTools::TriangulatePoly(FlagTriangles,FlagPolygon,true);
	// Todo 此方式在light.json文件转化时会有问题
	TArray<FVector2D> TrianVertices;
	FGeomTools2D::TriangulatePoly(TrianVertices,ShapeLine,true);
	
	// TArray<FVector2D> InputPolygon;
	// InputPolygon.Add(FVector2D(0,0));
	// InputPolygon.Add(FVector2D(1,0));
	// InputPolygon.Add(FVector2D(1,1));
	// InputPolygon.Add(FVector2D(0,1));
	// TArray<FVector2D> TrianVerticesTest;
	// FGeomTools2D::TriangulatePoly(TrianVerticesTest,InputPolygon,true);
	// for(FVector2D Vt : TrianVerticesTest)
	// {
	// 	UE_LOG(LogTemp,Log,TEXT("%s"),*(Vt.ToString()));
	// }
	
	
	auto Brush = FCoreStyle::Get().GetBrush("ColorWheel.HueValueCircle");
	auto Handle = Brush->GetRenderingResource();
	const FSlateShaderResourceProxy* ResourceProxy = Handle.GetResourceProxy();

	FVector2D UVCenter = FVector2D::ZeroVector;
	FVector2D UVRadius = FVector2D(1,1);
	if (ResourceProxy != nullptr)
	{
		UVRadius = 0.5f * ResourceProxy->SizeUV;
		UVCenter = ResourceProxy->StartUV + UVRadius;
	}

	int vtxid=0;
	for(auto vtx:TrianVertices)
	{
		FSlateVertex& NewVert = ShapeVertices[ShapeVertices.AddUninitialized()];

		// Copy Position
		{	
			NewVert.Position[0] = vtx.X+128;
			NewVert.Position[1] = vtx.Y+128;
		}
		// Copy Color
		{
			NewVert.Color = FColor::White;
		}
		// Copy all the UVs that we have, and as many as we can fit.
		{
			NewVert.TexCoords[0] = UVCenter.X;
			NewVert.TexCoords[1] = UVCenter.Y;
	
			NewVert.TexCoords[2] = 1.0f;
			NewVert.TexCoords[3] = 1.0f;
	
			// NewVert.MaterialTexCoords[0] = vtx.UVs[2].X;
			// NewVert.MaterialTexCoords[1] = vtx.UVs[2].Y;
		}
		ShapeIndexes.Add(vtxid);
		vtxid++;
	}
	
	// int vtxid=0;
	// for(auto trin:FlagTriangles)
	// {
	// 	for(auto vtx:trin.Vertices)
	// 	{
	// 		FSlateVertex& NewVert = ShapeVertices[ShapeVertices.AddUninitialized()];
	//
	// 		// Copy Position
	// 		{	
	// 			NewVert.Position[0] = vtx.Pos[0]*2+128;
	// 			NewVert.Position[1] = vtx.Pos[2]*2+128;
	// 		}
	// 		// Copy Color
	// 		{
	// 			NewVert.Color = vtx.Color;
	// 		}
	// 		// Copy all the UVs that we have, and as many as we can fit.
	// 		{
	// 			NewVert.TexCoords[0] = UVCenter.X;
	// 			NewVert.TexCoords[1] = UVCenter.Y;
	// 	
	// 			NewVert.TexCoords[2] = 1.0f;
	// 			NewVert.TexCoords[3] = 1.0f;
	// 	
	// 			NewVert.MaterialTexCoords[0] = vtx.UVs[2].X;
	// 			NewVert.MaterialTexCoords[1] = vtx.UVs[2].Y;
	// 		}
	// 		ShapeIndexes.Add(vtxid);
	// 		vtxid++;
	// 	}
	// }
}
