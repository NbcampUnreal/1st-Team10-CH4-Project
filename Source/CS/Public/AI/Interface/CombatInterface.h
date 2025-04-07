
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"


UINTERFACE()
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};


class CS_API ICombatInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int MeleeAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int Block();
};
