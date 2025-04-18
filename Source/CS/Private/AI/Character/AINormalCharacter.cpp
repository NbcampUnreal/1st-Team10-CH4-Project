#include "AI/Character/AINormalCharacter.h"
#include "AI/Controller/AINormalController.h"
#include "Components/CSCombatComponent.h"
#include "Kismet/GameplayStatics.h"

AAINormalCharacter::AAINormalCharacter()
{
	AIControllerClass = AAINormalController::StaticClass();
	Tags.Add(FName("Normal"));
}

void AAINormalCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!PatrolPath)
	{
		TArray<AActor*> FoundPaths;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolPath::StaticClass(), FoundPaths);

		for (AActor* Actor : FoundPaths)
		{
			if (Actor->ActorHasTag(FName("NomalPatrol")))
			{
				PatrolPath = Cast<APatrolPath>(Actor);
				break;
			}
		}
	}
}


void AAINormalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAINormalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FComboAttackData AAINormalCharacter::GetFirstAttackData() const
{
	FComboAttackData AttackData;

	const int32 RandomIndex = FMath::RandRange(0, 2);

	switch (RandomIndex)
	{
	case 0:
		AttackData.SectionName = FName("Punch1");
		break;
	case 1:
		AttackData.SectionName = FName("Punch2");
		break;
	case 2:
		AttackData.SectionName = FName("Punch3");
		break;
	}
	AttackData.TraceStart = FName("hand_r");
	AttackData.TraceEnd = FName("hand_l");
	AttackData.Damage = NomalAIDamage;
	AttackData.DType = ELaunchTypes::EDT_Nomal;
	return AttackData;
}

