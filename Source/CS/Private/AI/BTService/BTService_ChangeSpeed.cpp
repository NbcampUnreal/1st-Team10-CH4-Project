#include "AI/BTService/BTService_ChangeSpeed.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Change Speed";
}

void UBTService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	if (auto const Cont = OwnerComp.GetAIOwner())
	{
		if (auto* const AI = Cast<AAIBaseCharacter>(Cont->GetPawn()))
		{
			AI->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}
	}
}
