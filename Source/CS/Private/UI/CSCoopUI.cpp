// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSCoopUI.h"

void UCSCoopUI::NativeConstruct()
{
    Super::NativeConstruct();

    // ���� ��� ���� �ʱ�ȭ �۾�
    UE_LOG(LogTemp, Log, TEXT("CSCoopUI: NativeConstruct ȣ�� - ���� ��� �ʱ�ȭ"));
    // ��: ���� ����, ���� ��ǥ UI ���� �ʱ�ȭ
}

void UCSCoopUI::UpdateUI()
{
    Super::UpdateUI();

    // ���� ��� ���� UI ������Ʈ ����
    UE_LOG(LogTemp, Log, TEXT("CSCoopUI: UpdateUI ȣ�� - ���� ��� UI ������Ʈ"));
    // ��: �� ����, �÷��̾� ���� ������Ʈ ��
}