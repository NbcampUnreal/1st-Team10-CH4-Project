#include "UI/CSPlayerEntry.h"
#include "PlayerStates/CSPlayerState.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UCSPlayerEntry::NativeConstruct() 
{ 
    Super::NativeConstruct();
}

void UCSPlayerEntry::SetupPlayerEntry(ACSPlayerState* InPlayerState)
{
    AssociatedPlayerState = InPlayerState;
    if (InPlayerState) {
        UpdateEntryUI();
    }
    else { SetVisibility(ESlateVisibility::Collapsed); }
}