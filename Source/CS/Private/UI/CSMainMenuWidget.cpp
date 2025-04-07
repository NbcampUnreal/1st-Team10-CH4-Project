// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSMainMenuWidget.h"
#include "Components/Button.h"

void UCSMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 각 버튼이 제대로 바인딩되었는지 확인
    if (SoloModeButton)
    {
        SoloModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::HandleSoloModeClicked);
    }
    if (OnlineDuelModeButton)
    {
        OnlineDuelModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::HandleOnlineDuelClicked);
    }
    if (OnlineCoopModeButton)
    {
        OnlineCoopModeButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::HandleOnlineCoopClicked);
    }
    if (OptionsButton)
    {
        OptionsButton->OnClicked.AddDynamic(this, &UCSMainMenuWidget::HandleOptionsClicked);
    }

}

void UCSMainMenuWidget::HandleSoloModeClicked()
{
    //UE_LOG(LogTemp, Log, TEXT("솔로 모드 버튼 클릭됨"));
}

void UCSMainMenuWidget::HandleOnlineDuelClicked()
{
//    UE_LOG(LogTemp, Log, TEXT("온라인 대결 모드 버튼 클릭됨"));
     //온라인 대결 모드 선택
}

void UCSMainMenuWidget::HandleOnlineCoopClicked()
{
    //UE_LOG(LogTemp, Log, TEXT("온라인 협동 모드 버튼 클릭됨"));
    //온라인 협동 모드 선택
}

void UCSMainMenuWidget::HandleOptionsClicked()
{
    //UE_LOG(LogTemp, Log, TEXT("옵션창 버튼 클릭됨"));
    //옵션창 열기 관련 로직
}