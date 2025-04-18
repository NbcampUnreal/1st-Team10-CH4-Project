// Fill out your copyright notice in the Description page of Project Settings.


#include "CSAnimInstance.h"
#include "Characters/CSBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		BaseCharacter = Cast<ACSBaseCharacter>(Pawn);
	}
}

void UCSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (BaseCharacter == nullptr)
	{
		TryGetPawnOwner();
		if (!BaseCharacter.IsValid()) return;
	}

    ACSBaseCharacter* Character = BaseCharacter.Get();

    CurrentActionState = Character->GetActionState();
    bIsAlive = Character->IsAlive();

    FVector Velocity = Character->GetVelocity();
    Velocity.Z = 0;
    Speed = Velocity.Size();

    UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
    if (MovementComponent)
    {
        bIsInAir = MovementComponent->IsFalling();
        bIsAccelerating = MovementComponent->GetCurrentAcceleration().Size() > 0.f;
    }
    else
    {
        bIsInAir = false;
        bIsAccelerating = false;
    }

    bIsCrouched = Character->bIsCrouched;
}


