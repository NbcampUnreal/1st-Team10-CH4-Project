
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
	int FirstAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int SecondAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int LowComboAttack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int RangeComboAttack();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int Block();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	int Rolling();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int Dodge(AActor* Attacker);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int RunAway(AActor* Attacker);
};
