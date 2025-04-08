// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSUIBaseWidget.h"

void UCSUIBaseWidget::UpdateReadyUI_Implementation(bool bIsReady)
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::UpdateTeamUI_Implementation(int32 TeamID)
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::UpdateCharacterUI_Implementation(EJobTypes NewJob)
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::UpdateSelectedMapUI_Implementation(FName SelectedMap)
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::UpdateMatchTimeUI_Implementation(int32 Time)
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::OnMatchPhaseChanged_Implementation()
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}

void UCSUIBaseWidget::OnSuddenDeathUI_Implementation()
{
    // 부모에서는 아무런 로직도 구현하지 않음.
}