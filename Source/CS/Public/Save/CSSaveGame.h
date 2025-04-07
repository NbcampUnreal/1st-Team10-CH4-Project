#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CSSaveGame.generated.h"

UCLASS()
class CS_API UCSSaveGame : public USaveGame
{
    GENERATED_BODY()

public: // Public으로 선언되어 있는지 확인
    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName; // 스펠링, 대소문자 확인

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex; // 스펠링, 대소문자 확인

    UPROPERTY(VisibleAnywhere, Category = Gameplay)
    TSet<FName> UnlockedCharacterIDs; // 스펠링, 대소문자 확인

    UCSSaveGame();
};