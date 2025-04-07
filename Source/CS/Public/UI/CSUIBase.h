// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSUIBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CS_API UCSUIBase : public UUserWidget
{
	GENERATED_BODY()

public:
    // UI�� ������ �� ȣ��Ǵ� �Լ� (�ʱ�ȭ �ڵ�)
    virtual void NativeConstruct() override;

    // ����: ��ġ ���� ���濡 ���� UI�� ������Ʈ�ϴ� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void UpdateUI();
};
