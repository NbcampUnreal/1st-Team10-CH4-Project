#include "AI/Character/AINormalCharacter.h"
#include "AI/Controller/AINormalController.h"

AAINormalCharacter::AAINormalCharacter()
{
	AIControllerClass = AAINormalController::StaticClass();
}
