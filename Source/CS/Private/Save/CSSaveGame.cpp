#include "Save/CSSaveGame.h"

UCSSaveGame::UCSSaveGame()
{
    SaveSlotName = TEXT("PlayerProgress"); // 기본 슬롯 이름
    UserIndex = 0;
    // 기본적으로 해금된 캐릭터가 있다면 여기에 추가
    // UnlockedCharacterIDs.Add(FName("DefaultCharacterID"));
}