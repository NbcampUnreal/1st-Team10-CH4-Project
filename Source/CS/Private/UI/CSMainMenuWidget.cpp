// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSMainMenuWidget.h"
#include "Components/Button.h"

void UCSMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // �� ��ư�� ����� ���ε��Ǿ����� Ȯ��
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
    //UE_LOG(LogTemp, Log, TEXT("�ַ� ��� ��ư Ŭ����"));
}

void UCSMainMenuWidget::HandleOnlineDuelClicked()
{
//    UE_LOG(LogTemp, Log, TEXT("�¶��� ��� ��� ��ư Ŭ����"));
     //�¶��� ��� ��� ����
}

void UCSMainMenuWidget::HandleOnlineCoopClicked()
{
    //UE_LOG(LogTemp, Log, TEXT("�¶��� ���� ��� ��ư Ŭ����"));
    //�¶��� ���� ��� ����
}

void UCSMainMenuWidget::HandleOptionsClicked()
{
    //UE_LOG(LogTemp, Log, TEXT("�ɼ�â ��ư Ŭ����"));
    //�ɼ�â ���� ���� ����
}