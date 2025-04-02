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
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // MatchType이 선택된 후 GameInstance 값을 기반으로 맵을 연다 (서버에서만 호출됨)
    // PlayerController에서 Server RPC로 이 함수를 호출하면, 선택된 MatchType으로 Level 전환됨
    // Server RPC의 경우엔 예를들어 RequestStartMatch 함수가 있다고 하면 거기서 GameMdoe->TryStartMatch를 호출하는 방식
    void TryStartMatch();

protected:
    // 메인 메뉴 UI 클래스 설정 (GameMode에서 Controller에게 전달할 용도)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;
};