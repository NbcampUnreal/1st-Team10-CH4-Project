// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSDuelUI.h"

void UCSDuelUI::NativeConstruct()
{
    Super::NativeConstruct();

    // ��� ��� ���� �ʱ�ȭ �۾�
    UE_LOG(LogTemp, Log, TEXT("UCSDuelUI: NativeConstruct ȣ�� - ��� ��� �ʱ�ȭ"));
}

void UCSDuelUI::UpdateUI()
{
    Super::UpdateUI();

    // ��� ��� ���� UI ������Ʈ ����
    UE_LOG(LogTemp, Log, TEXT("UCSDuelUI: UpdateUI ȣ�� - ��� ��� UI ������Ʈ"));
    // ��: ����, Ÿ�̸� ���� �� ����
}