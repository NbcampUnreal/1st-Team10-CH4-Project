#include "AI/Character/AIBossCharacter.h"
#include "AI/Controller/AIBossController.h"

AAIBossCharacter::AAIBossCharacter()
{
	AIControllerClass = AAIBossController::StaticClass();
}


void AAIBossCharacter::OnHitReaction()
{
	CurrentState = EBossAIState::Defensive;

	// 일정 시간 후 반격 상태로 전환
	GetWorldTimerManager().SetTimer(TimerHandle_CounterAttack, this, &AAIBossCharacter::EnterCounterAttack, DefenseTime, false);
}

void AAIBossCharacter::EnterCounterAttack()
{
	CurrentState = EBossAIState::CounterAttack;
}



