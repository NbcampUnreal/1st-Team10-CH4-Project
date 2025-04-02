#include "AI/Character/AIBossCharacter.h"
#include "AI/Controller/AIBossController.h"

AAIBossCharacter::AAIBossCharacter()
{
	AIControllerClass = AAIBossController::StaticClass();
}
