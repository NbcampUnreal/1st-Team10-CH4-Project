#include "AI/Character/AIBaseCharacter.h"

AAIBaseCharacter::AAIBaseCharacter()
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
