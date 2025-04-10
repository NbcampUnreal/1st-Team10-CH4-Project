#include "AI/Character/AIBossCharacter.h"
#include "NavigationSystem.h"
#include "AI/Controller/AIBossController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CSCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

AAIBossCharacter::AAIBossCharacter()
{
	AIControllerClass = AAIBossController::StaticClass();
	Tags.Add(FName("Boss"));
}

void AAIBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}




