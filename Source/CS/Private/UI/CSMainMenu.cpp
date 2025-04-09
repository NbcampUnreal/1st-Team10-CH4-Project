#include "UI/CSMainMenu.h" // 경로 확인
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "TimerManager.h" // 알림 타이머

void UCSMainMenu::NativeConstruct()
{
    Super::NativeConstruct();
    if (ArcadeModeButton) ArcadeModeButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnArcadeModeClicked);
    if (VersusModeButton) VersusModeButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnVersusModeClicked);
    if (CoopModeButton) CoopModeButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnCoopModeClicked);
    if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnExitClicked);

    if (NotificationText) NotificationText->SetVisibility(ESlateVisibility::Collapsed); // 처음엔 숨김
}

void UCSMainMenu::NativeDestruct()
{
    // 위젯 소멸 시 타이머 클리어
    GetWorld()->GetTimerManager().ClearTimer(NotificationTimerHandle);
    Super::NativeDestruct();
}

void UCSMainMenu::OnArcadeModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Arcade Mode Clicked"));
    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    if (GI) {
        GI->SetMatchType(EMatchType::EMT_Single);
        // GI->StartingSinglePlay(); // GameInstance 함수 사용 (LevelData 필요)
        UGameplayStatics::OpenLevel(GetWorld(), FName("StageLevel")); // 또는 직접 레벨 열기
        UE_LOG(LogTemp, Log, TEXT("Opening StageLevel directly for Arcade Mode."));
    }
    else { UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance for Arcade Mode.")); }
}

void UCSMainMenu::OnVersusModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Versus Mode Clicked"));
    FindOrCreateMultiplayerSession(EMatchType::EMT_Versus);
}

void UCSMainMenu::OnCoopModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Coop Mode Clicked"));
    FindOrCreateMultiplayerSession(EMatchType::EMT_Coop);
}

void UCSMainMenu::FindOrCreateMultiplayerSession(EMatchType MatchType)
{
    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    if (GI)
    {
        // 버튼 비활성화 (처리 중 표시)
        if (VersusModeButton) VersusModeButton->SetIsEnabled(false);
        if (CoopModeButton) CoopModeButton->SetIsEnabled(false);
        ShowNotification(FText::FromString(TEXT("Searching for sessions...")), 0.0f); // 0 duration = 수동으로 숨길 때까지

        // GameInstance의 세션 찾기/생성 함수 호출 (내부적으로 Find -> Host 로직 수행)
        GI->FindOrCreateSession(MatchType); // GameInstance에 이 함수 구현 필요

        // 실제 세션 처리 콜백은 GameInstance에서 처리 후 결과에 따라 PlayerController RPC 호출
        // 예: 세션 못찾고 생성도 실패 -> Client_ShowNoSessionPopup 호출 -> 이 위젯의 ShowNotification 실행
        // 예: 참가/호스트 성공 -> 서버/클라이언트 트래블 -> LobbyLevel 로딩
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance not found!"));
        ShowNotification(FText::FromString(TEXT("Error: GameInstance not found!")));
    }
    // 버튼 다시 활성화는 GameInstance 콜백 처리 후(성공/실패 무관) 필요할 수 있음
    // 임시로 약간의 딜레이 후 활성화 (좋은 방식은 아님)
    // FTimerHandle TempHandle;
    // GetWorldTimerManager().SetTimer(TempHandle, [this](){
    //     if(VersusModeButton) VersusModeButton->SetIsEnabled(true);
    //     if(CoopModeButton) CoopModeButton->SetIsEnabled(true);
    //     HideNotification();
    // }, 5.0f, false); // 5초 후
}

void UCSMainMenu::ShowNotification(const FText& Message, float Duration)
{
    if (NotificationText)
    {
        NotificationText->SetText(Message);
        NotificationText->SetVisibility(ESlateVisibility::HitTestInvisible); // 보이도록 설정

        // 타이머 클리어 (이전 알림이 있다면)
        GetWorld()->GetTimerManager().ClearTimer(NotificationTimerHandle);

        // Duration이 0보다 크면 일정 시간 후 숨김
        if (Duration > 0.0f)
        {
            GetWorld()->GetTimerManager().SetTimer(NotificationTimerHandle, this, &UCSMainMenu::HideNotification, Duration, false);
        }
        // 버튼 재활성화 로직도 여기에 넣는 것이 좋을 수 있음
        if (Duration > 0.0f) { // 에러 메시지 표시 후 버튼 활성화
            if (VersusModeButton) VersusModeButton->SetIsEnabled(true);
            if (CoopModeButton) CoopModeButton->SetIsEnabled(true);
        }
    }
}

void UCSMainMenu::HideNotification()
{
    if (NotificationText)
    {
        NotificationText->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UCSMainMenu::OnExitClicked()
{
    UE_LOG(LogTemp, Log, TEXT("Exit Clicked"));
    APlayerController* PC = GetOwningPlayer();
    if (PC) PC->ConsoleCommand("quit");
}