// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CSSoloUI.h"

void UCSSoloUI::NativeConstruct()
{
    Super::NativeConstruct();

    // �ַ� ��� ���� �ʱ�ȭ �۾�
    UE_LOG(LogTemp, Log, TEXT("CSSingleUI: NativeConstruct ȣ�� - �ַ� ��� �ʱ�ȭ"));
    // ��: �ַ� ��� ������, �÷��̾� ���� ���� �ʱ�ȭ
}

void UCSSoloUI::UpdateUI()
{
    Super::UpdateUI();

    // �ַ� ��� ���� UI ������Ʈ ����
    UE_LOG(LogTemp, Log, TEXT("CSSingleUI: UpdateUI ȣ�� - �ַ� ��� UI ������Ʈ"));
    // ��: ����, Ÿ�̸� ���� �� ����
}