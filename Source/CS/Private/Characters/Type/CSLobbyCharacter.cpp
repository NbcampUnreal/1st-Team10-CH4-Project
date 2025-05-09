#include "Characters/Type/CSLobbyCharacter.h"
#include "CSTypes/CSCharacterTypes.h"
#include "GameInstance/CSAdvancedGameInstance.h"

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

void ACSLobbyCharacter::Multicast_UpdateMesh_Implementation(EJobTypes NewJob)
{
    UpdateMeshFromJobType(NewJob);
}

void ACSLobbyCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ACSLobbyCharacter::UpdateMeshFromJobType(EJobTypes JobType)
{
    FString EnumName = StaticEnum<EJobTypes>()->GetNameStringByValue((int64)JobType);
    EnumName.RemoveFromStart(TEXT("EJT_")); 
    FName RowName = FName(*EnumName);

    static const FString ContextString(TEXT("CharacterLobbyData"));
    FCharacterLobbyData* RowData = CharacterLobbyDataTable->FindRow<FCharacterLobbyData>(RowName, ContextString);

    if (!RowData || !GetMesh()) return;

    USkeletalMesh* NewMesh = RowData->LobbyMesh.LoadSynchronous();
    TSubclassOf<UAnimInstance> NewAnimClass = RowData->LobbyAnimClass;

    GetMesh()->SetSkeletalMesh(NewMesh);
    GetMesh()->SetAnimInstanceClass(NewAnimClass);
}
