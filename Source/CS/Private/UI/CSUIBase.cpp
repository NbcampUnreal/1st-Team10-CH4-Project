// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSUIBase.h"


void UCSUIBase::NativeConstruct()
{
    Super::NativeConstruct();

    // 공통 초기화 작업: 로딩 애니메이션, 기본 변수 초기화 등
}

void UCSUIBase::UpdateUI()
{
    // 공통 UI 업데이트 로직 (필요에 따라 자식 클래스에서 오버라이드)
}