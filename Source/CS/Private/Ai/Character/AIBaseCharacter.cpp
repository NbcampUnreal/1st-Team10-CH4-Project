#include "AI/Character/AIBaseCharacter.h"

AAIBaseCharacter::AAIBaseCharacter() : WidgetComponent{
{

}

void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
};
void AAIBaseCharacter::SetupPlayerInputComponent(class UInputComponent* AIInputComponent)
{
	Super::SetupPlayerInputComponent(AIInputComponent);
};

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
};

UBehaviorTree* AAIBaseCharacter::GetBehaviorTree() const
{
	return Tree;
}

APatrolPath* AAIBaseCharacter::GetPatrolPath() const
{
	return PatrolPath;
}

UAnimMontage* AAIBaseCharacter::GetMontage() const
{
	if (AttackMontages.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AttackMontages.Num() - 1);
		return AttackMontages[Index];
	}
	return nullptr;
}

int AAIBaseCharacter::MeleeAttack_Implementation()
{
	UAnimMontage* SelectedMontage = GetMontage();
	if (SelectedMontage)
	{
		PlayAnimMontage(SelectedMontage);
	}
	return 0;
}
