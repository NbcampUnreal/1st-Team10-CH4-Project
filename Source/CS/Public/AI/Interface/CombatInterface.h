
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
	int firstAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int secondAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int LowComboAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int RangeComboAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int Block();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int Dodge(AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int RunAway(AActor* Attacker);
};
