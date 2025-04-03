#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSMainMenuGameMode.generated.h"

UCLASS()
class CS_API ACSMainMenuGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    // MatchType이 선택된 후 GameInstance 값을 기반으로 맵을 연다 (서버에서만 호출됨)
    // PlayerController에서 Server RPC로 이 함수를 호출하면, 선택된 MatchType으로 Level 전환됨
    // Server RPC의 경우엔 예를들어 RequestStartMatch 함수가 있다고 하면 거기서 GameMode->TryStartMatch를 호출하는 방식
    void TryStartMatch();
};