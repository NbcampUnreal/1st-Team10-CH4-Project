// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSMainMenuWidget.h"
#include "Components/Button.h"


void UCSMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 각 버튼이 제대로 바인딩되었는지 확인 후 클릭 이벤트 연결
    if (SingleModeButton)
    {
        SingleModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::SingleModeClicked);
    }
    if (VersusModeButton)
    {
        VersusModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::VersusModeClicked);
    }
    if (CoopModeButton)
    {
        CoopModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::CoopModeClicked);
    }
    if (OptionsButton)
    {
        OptionsButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::OptionsClicked);
    }
}

void UCSMainMenuWidget::SingleModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("솔로 모드 버튼 클릭됨"));
    // 솔로 모드 관련 로직 실행
}

void UCSMainMenuWidget::VersusModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("온라인 대결 모드 버튼 클릭됨"));
    // 온라인 대결 모드 관련 로직 실행
}

void UCSMainMenuWidget::CoopModeClicked()
{
    UE_LOG(LogTemp, Log, TEXT("온라인 협동 모드 버튼 클릭됨"));
    // 온라인 협동 모드 관련 로직 실행
}

void UCSMainMenuWidget::OptionsClicked()
{
    UE_LOG(LogTemp, Log, TEXT("옵션 버튼 클릭됨"));
    // 옵션창 관련 로직 실행
}