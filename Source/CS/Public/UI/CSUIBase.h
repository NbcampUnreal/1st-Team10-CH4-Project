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
    // UI가 생성될 때 호출되는 함수 (초기화 코드)
    virtual void NativeConstruct() override;

    // 예시: 매치 상태 변경에 따라 UI를 업데이트하는 공통 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void UpdateUI();
};
