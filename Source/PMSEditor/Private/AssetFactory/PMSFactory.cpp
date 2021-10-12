#include "AssetFactory/PMSFactory.h"

//From PMS
#include "PMSGraph.h"

UPMSFactory::UPMSFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UPMS_Graph::StaticClass();
}

UObject* UPMSFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UPMS_Graph::StaticClass()));
	return NewObject<UPMS_Graph>(InParent, Class, Name, Flags);
}