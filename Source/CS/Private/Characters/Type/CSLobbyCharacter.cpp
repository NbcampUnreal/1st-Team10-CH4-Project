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
}

void ACSLobbyCharacter::UpdateMeshFromJobType(EJobTypes JobType)
{
	// Tranfer JobType to FName for DataTable lookup
    FName RowName = *UEnum::GetDisplayValueAsText(JobType).ToString(); // "EJT_Fighter"

	// if failed, print ContextString
    static const FString ContextString(TEXT("CharacterLobbyData"));
    FCharacterLobbyData* RowData = CharacterLobbyDataTable->FindRow<FCharacterLobbyData>(RowName, ContextString);

    if (RowData)
    {
        USkeletalMesh* NewMesh = RowData->LobbyMesh.LoadSynchronous();
        TSubclassOf<UAnimInstance> NewAnimClass = RowData->LobbyAnimClass;

        if (GetMesh())
        {
            GetMesh()->SetSkeletalMesh(NewMesh);
            GetMesh()->SetAnimInstanceClass(NewAnimClass);
        }
    }
    else
    {
        if (GetMesh())
        {
            GetMesh()->SetSkeletalMesh(nullptr);
            GetMesh()->SetAnimInstanceClass(nullptr);
        }
    }
}
