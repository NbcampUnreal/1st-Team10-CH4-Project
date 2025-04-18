#include "AI/Character/AIFighterCharacter.h"

#include "AI/Controller/AIFighterController.h"
#include "Kismet/GameplayStatics.h"


AAIFighterCharacter::AAIFighterCharacter()
{
	AIControllerClass = AAIFighterController::StaticClass();
	Tags.Add(FName("Fighter"));
}


void AAIFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!PatrolPath)
	{
		TArray<AActor*> FoundPaths;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolPath::StaticClass(), FoundPaths);

		for (AActor* Actor : FoundPaths)
		{
			if (Actor->ActorHasTag(FName("CoopPatrol")))
			{
				PatrolPath = Cast<APatrolPath>(Actor);
				break;
			}
		}
	}
}


void AAIFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAIFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


