#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "PMSFactory.generated.h"

UCLASS(HideCategories=Object, MinimalAPI)
class UPMSFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
public:
	UPMSFactory();


	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};