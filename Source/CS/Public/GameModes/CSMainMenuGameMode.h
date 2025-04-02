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

    // MatchType�� ���õ� �� GameInstance ���� ������� ���� ���� (���������� ȣ���)
    // PlayerController���� Server RPC�� �� �Լ��� ȣ���ϸ�, ���õ� MatchType���� Level ��ȯ��
    // Server RPC�� ��쿣 ������� RequestStartMatch �Լ��� �ִٰ� �ϸ� �ű⼭ GameMdoe->TryStartMatch�� ȣ���ϴ� ���
    void TryStartMatch();

protected:
    // ���� �޴� UI Ŭ���� ���� (GameMode���� Controller���� ������ �뵵)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;
};