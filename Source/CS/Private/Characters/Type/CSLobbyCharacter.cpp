// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Type/CSLobbyCharacter.h"
#include "CSTypes/CSCharacterTypes.h"
#include "GameInstance/CSGameInstance.h"

ACSLobbyCharacter::ACSLobbyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    bAlwaysRelevant = true;
    SetReplicateMovement(true);

    if (GetMesh())
    {
        GetMesh()->SetIsReplicated(true);
    }
}

void ACSLobbyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ACSLobbyCharacter::UpdateMeshFromJobType(EJobTypes JobType)
{
    FName RowName = *UEnum::GetDisplayValueAsText(JobType).ToString();

    static const FString ContextString(TEXT("CharacterLobbyData"));
    FCharacterLobbyData* RowData = CharacterLobbyDataTable->FindRow<FCharacterLobbyData>(RowName, ContextString);

    if (!RowData || !GetMesh()) return;

    USkeletalMesh* NewMesh = RowData->LobbyMesh.LoadSynchronous();
    TSubclassOf<UAnimInstance> NewAnimClass = RowData->LobbyAnimClass;

    GetMesh()->SetSkeletalMesh(NewMesh);
    GetMesh()->SetAnimInstanceClass(NewAnimClass);
}
