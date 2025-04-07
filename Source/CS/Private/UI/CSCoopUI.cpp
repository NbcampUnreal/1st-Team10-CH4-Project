// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSCoopUI.h"

void UCSCoopUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 협동 모드 전용 초기화 작업
    UE_LOG(LogTemp, Log, TEXT("CSCoopUI: NativeConstruct 호출 - 협동 모드 초기화"));
    // 예: 팀원 정보, 공동 목표 UI 등을 초기화
}

void UCSCoopUI::UpdateUI()
{
    Super::UpdateUI();

    // 협동 모드 전용 UI 업데이트 로직
    UE_LOG(LogTemp, Log, TEXT("CSCoopUI: UpdateUI 호출 - 협동 모드 UI 업데이트"));
    // 예: 팀 점수, 플레이어 상태 업데이트 등
}