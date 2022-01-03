#include "AssetFactory/PMSFactory.h"

//From PMS
#include "PMSGraph.h"
#include "Editor/PMSEdGraph.h"

UPMSFactory::UPMSFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPMSEdGraph::StaticClass();
}

UObject* UPMSFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UPMSEdGraph::StaticClass()));
	return NewObject<UPMSEdGraph>(InParent, Class, Name, Flags);
}