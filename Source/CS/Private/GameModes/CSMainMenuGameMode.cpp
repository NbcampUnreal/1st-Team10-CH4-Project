#include "GameModes/CSMainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CSGameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Controller/CSPlayerController.h"
#include "CSTypes/CSGameTypes.h"

void ACSMainMenuGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ACSMainMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (ACSPlayerController* PC = Cast<ACSPlayerController>(NewPlayer))
    {
        // PlayerController에게 메인 메뉴 UI를 표시하도록 클라이언트 RPC 호출
        // 어떤 위젯을 표시할지는 PlayerController가 결정
        // PC->Client_ShowMainMenuUI();
    }
}

void ACSMainMenuGameMode::TryStartMatch()
{
    if (const UCSGameInstance* GI = GetGameInstance<UCSGameInstance>())
    {
        switch (GI->MatchType)
        {
        case EMatchType::EMT_Single:
            UGameplayStatics::OpenLevel(this, FName("TutorialLevel"));
            break;
        case EMatchType::EMT_Versus:
        case EMatchType::EMT_Coop:
            UGameplayStatics::OpenLevel(this, FName("LobbyLevel"));
            break;
        default:
            UE_LOG(LogTemp, Warning, TEXT("[MainMenu] Invalid MatchType set in GameInstance."));
            break;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[MainMenu] Failed to get GameInstance. Cannot start match."));
    }
}