// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/CSSpawnProjectileNotify.h"
#include "Characters/CSPlayerCharacter.h"

void UCSSpawnProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* ActorCheck = MeshComp->GetOwner();

	if (ActorCheck) 
	{
		ACSPlayerCharacter* Player = Cast<ACSPlayerCharacter>(ActorCheck);
		if (Player)
		{
			Player->ServerSpawnProjectile(Player);
		}
	}
}
