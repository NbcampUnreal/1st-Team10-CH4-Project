#pragma once
#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h"
#include "CSBossGameState.generated.h"

UCLASS()
class CS_API ACSBossGameState : public ACSGameStateBase
{
    GENERATED_BODY()
public:
    ACSBossGameState();
    // 필요시 보스 체력 % 등 리플리케이트 변수 추가
};