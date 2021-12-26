// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PMSEditorSettings.generated.h"

/**
 * 
 */
// USTRUCT()
// struct PMSEDITOR_API FViewportSettings
// {
// 	GENERATED_BODY()
// 	
// 	// If true, displays a border around the texture (configured via the material editor)
// 	UPROPERTY()
// 	bool bShowBorder;
//
// 	// Color to use for the border, if enabled
// 	UPROPERTY(EditAnywhere, Category = "Background")
// 	FColor BorderColor;
//
// 	// The type of background to show (configured via the material editor)
// 	UPROPERTY()
// 	EBackgroundType BackgroundType;
//
// 	// The color used as the background of the preview
// 	UPROPERTY(EditAnywhere, Category = "Background")
// 	FColor BackgroundColor;
//
// 	// Note: For now these defaults match the historical material editor behavior, not the texture editor's defaults
// 	FViewportSettings()
// 		: bShowBorder(false)
// 		, BorderColor(FColor::White)
// 		, BackgroundType(EBackgroundType::SolidColor)
// 		, BackgroundColor(FColor::Black)
// 	{
//
// 	}
//
// };

UCLASS(config = EditorPerProjectUserSettings)
class UPMSEditorSettings : public UObject
{
	GENERATED_BODY()
	
public:
	/**
	Path to user installed Mali shader compiler that can be used by the material editor to compile and extract shader informations for Android platforms.
	Official website address: https://developer.arm.com/products/software-development-tools/graphics-development-tools/mali-offline-compiler/downloads
	*/
	//UPROPERTY(config, EditAnywhere, Category = "Offline Shader Compilers", meta = (DisplayName = "Mali Offline Compiler"))
	//FFilePath MaliOfflineCompilerPath;

public:
	// The width (in pixels) of the preview viewport when a material editor is first opened
	UPROPERTY(config, EditAnywhere, meta=(ClampMin=1, ClampMax=4096), Category="Graph Editor Setting")
	int32 GridSize = 100;

	// The height (in pixels) of the preview viewport when a material editor is first opened
	UPROPERTY(config, EditAnywhere, meta=(ClampMin=1, ClampMax=4096), Category="Graph Editor Setting")
	int32 SubGridCount = 1;

	UPROPERTY(config, EditAnywhere, meta=(ClampMin=1, ClampMax=4096), Category="Graph Editor Setting")
	int32 SnappingDistance = 16;

	UPROPERTY(config, EditAnywhere, Category="Debug")
	bool EnablePreProcesser = false;
public:
	// Configures the background shown behind the UI material preview
	//UPROPERTY(config, EditAnywhere, Category = "User Interface Domain")
	//FPreviewBackgroundSettings PreviewBackground;

	// FIntPoint GetPreviewViewportStartingSize() const
	// {
	// 	return FIntPoint(DefaultPreviewWidth, DefaultPreviewHeight);
	// }

#if WITH_EDITOR
	// Allow listening for changes just to this settings object without having to listen to all UObjects
	FSimpleMulticastDelegate OnPostEditChange;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		OnPostEditChange.Broadcast();
	}
#endif // WITH_EDITOR
};
