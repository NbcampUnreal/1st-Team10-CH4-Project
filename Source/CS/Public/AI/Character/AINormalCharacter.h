#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AINormalCharacter.generated.h"

UCLASS()
class CS_API AAINormalCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAINormalCharacter();
	virtual FComboAttackData GetFirstAttackData() const override;
	int AI_Attack(UAnimMontage* SelectedMontage, const FComboAttackData& AttackData);
};
