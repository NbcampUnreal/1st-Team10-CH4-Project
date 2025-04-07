#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AIBossCharacter.generated.h"

UENUM(BlueprintType)
enum class EBossAIState : uint8
{
	Aggressive,
	Defensive,
	CounterAttack,
	Evade,
	Idle,
};

UCLASS()
class CS_API AAIBossCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAIBossCharacter();
	
	UPROPERTY(BlueprintReadWrite, Category="AI State")
	EBossAIState CurrentState;

	void OnHitReaction();
	void EnterCounterAttack();

	float DefenseTime = 1.0f;
private:
	FTimerHandle TimerHandle_CounterAttack;
};
