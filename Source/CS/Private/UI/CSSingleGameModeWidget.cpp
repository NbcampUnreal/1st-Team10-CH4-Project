// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSSingleGameModeWidget.h"

void UCSSingleGameModeWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 솔로 모드 전용 초기화 작업
    UE_LOG(LogTemp, Log, TEXT("CSSingleUI: NativeConstruct 호출 - 솔로 모드 초기화"));
    // 예: 솔로 모드 점수판, 플레이어 상태 등을 초기화
}

