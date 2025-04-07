// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CSPerformHitNotify.h"

#include "Characters/CSBaseCharacter.h"
#include "Components/CSCombatComponent.h"

void UCSPerformHitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner)
	{
		ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(Owner);
		if (Character)
		{
			UCSCombatComponent* CombatComp = Character->FindComponentByClass<UCSCombatComponent>();
			if (CombatComp)
			{
				CombatComp->Server_PerformHitCheck();
			}
		}
	}
}