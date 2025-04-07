// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSDuelUI.h"

void UCSDuelUI::NativeConstruct()
{
    Super::NativeConstruct();

    // 대결 모드 전용 초기화 작업
    UE_LOG(LogTemp, Log, TEXT("UCSDuelUI: NativeConstruct 호출 - 대결 모드 초기화"));
}

void UCSDuelUI::UpdateUI()
{
    Super::UpdateUI();

    // 대결 모드 전용 UI 업데이트 로직
    UE_LOG(LogTemp, Log, TEXT("UCSDuelUI: UpdateUI 호출 - 대결 모드 UI 업데이트"));
    // 예: 점수, 타이머 등의 값 갱신
}