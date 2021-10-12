#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Templates/SubclassOf.h"

#include "PMSGraph.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PMS_API UPMS_Graph : public UObject
{
	GENERATED_BODY()
		
public:

//#if WITH_EDITORONLY_DATA
public:
	UPROPERTY()
	class UEdGraph* EdGraph;

	//template <class T>
	//T* SpawnNodeInsideGraph()
//#endif
};