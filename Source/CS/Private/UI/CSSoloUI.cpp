// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSSoloUI.h"

void UCSSoloUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 솔로 모드 전용 초기화 작업
    UE_LOG(LogTemp, Log, TEXT("CSSingleUI: NativeConstruct 호출 - 솔로 모드 초기화"));
    // 예: 솔로 모드 점수판, 플레이어 상태 등을 초기화
}

void UCSSoloUI::UpdateUI()
{
    Super::UpdateUI();

    // 솔로 모드 전용 UI 업데이트 로직
    UE_LOG(LogTemp, Log, TEXT("CSSingleUI: UpdateUI 호출 - 솔로 모드 UI 업데이트"));
    // 예: 점수, 타이머 등의 값 갱신
}