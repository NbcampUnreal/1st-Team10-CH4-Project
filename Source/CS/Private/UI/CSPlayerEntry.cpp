#include "UI/CSPlayerEntry.h" // 경로 확인
#include "PlayerStates/CSPlayerState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCSPlayerEntry::NativeConstruct() { Super::NativeConstruct(); /* 초기화 */ }

void UCSPlayerEntry::SetupPlayerEntry(ACSPlayerState* InPlayerState)
{
    AssociatedPlayerState = InPlayerState;
    if (InPlayerState) {
        UpdateEntryUI(); // BP 구현 이벤트 호출
        // C++ 예시:
        // if(PlayerNameText) PlayerNameText->SetText(FText::FromString(InPlayerState->GetPlayerName()));
        // if(ReadyStatusText) ReadyStatusText->SetText(FText::FromString(InPlayerState->bIsReady ? "Ready" : "")); // 준비 안됨은 빈칸
        // if(JobText) {
        //     const UEnum* EnumPtr = StaticEnum<EJobTypes>();
        //     FString JobString = EnumPtr ? EnumPtr->GetNameStringByValue((int64)InPlayerState->SelectedJob) : FString("None");
        //     JobText->SetText(FText::FromString(JobString));
        // }
    }
    else { SetVisibility(ESlateVisibility::Collapsed); }
}