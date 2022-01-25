// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PMSEDITOR_API FNodeShape
{
public:
	FNodeShape();
	explicit FNodeShape(FString FilePath);
	
	~FNodeShape();	

	TArray<TArray<FVector2D>> FlagsLine;
	TArray<FSlateVertexArray> FlagsVertices;
	TArray<FSlateIndexArray> FlagsIndexes;
	
	TArray<FString> FlagNames;

	TArray<FVector2D> OutLine;
	FSlateVertexArray OutLineVertices;	
	FSlateIndexArray OutlineIndex;
	
	TArray<FVector2D> Input;
	TArray<FVector2D> OutPut;
	TArray<FVector2D> Icon;

	FName NodeShapeName;
	float Scale;

	void UpdataByJsonValue(TArray<FVector2D>& ShapeLine, FSlateVertexArray& ShapeVertices, FSlateIndexArray& ShapeIndexes, TArray<TSharedPtr<FJsonValue>> JsonArray);
};

class FNodeShapeCollection
{
public:
	FNodeShapeCollection()
	{
		if(!bInitialized)
		{
			TArray<FString> NodeShapePaths;
			IFileManager::Get().FindFilesRecursive(NodeShapePaths,*(FPaths::ProjectPluginsDir()/TEXT("PMS/Resources/NodeShapes")),TEXT("*.json"),true,false);
			for (FString NodeShapePath : NodeShapePaths)
			{
				FNodeShape NodeShape(NodeShapePath);
				NodeShapes.Add(NodeShape.NodeShapeName,NodeShape);
			}
		}		
	};
	static TSharedPtr<FNodeShapeCollection> Get(){return Instance;}
	static TSharedRef<FNodeShapeCollection> Create(){return MakeShareable(new FNodeShapeCollection());};
	static void Initialize(){if(!Instance.IsValid()){Instance = Create();bInitialized = true;}};
	static void Shutdown(){Instance.Reset();}
	FNodeShape* GetNodeShapeByName(FName NodeShapeName)
	{
		return NodeShapes.Find(NodeShapeName);
	}
	
private:
	static TSharedPtr<FNodeShapeCollection> Instance;
	static bool bInitialized;
	TMap<FName,FNodeShape> NodeShapes;
};
