// Fill out your copyright notice in the Description page of Project Settings.


#include "CSAnimInstance.h"
#include "Characters/CSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<ACSPlayerCharacter>(TryGetPawnOwner());
}

void UCSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (PlayerCharacter == nullptr)
	{
		PlayerCharacter = Cast<ACSPlayerCharacter>(TryGetPawnOwner());
	}

	if (PlayerCharacter == nullptr) return;

	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsCrouched = PlayerCharacter->bIsCrouched;
	//ActionState = PlayerCharacter->ActionState;
}


