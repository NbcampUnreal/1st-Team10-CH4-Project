// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "Components/Button.h"
#include "CSMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UCSMainMenuWidget : public UCSUIBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
    // UMG에서 디자인한 버튼들을 BindWidget으로 연결
    UPROPERTY(meta = (BindWidget))
    UButton* SingleModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* VersusModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* CoopModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OptionsButton;

    // 버튼 클릭 이벤트 핸들러
    UFUNCTION()
    void SingleModeClicked();

    UFUNCTION()
    void VersusModeClicked();

    UFUNCTION()
    void CoopModeClicked();

    UFUNCTION()
    void OptionsClicked();
};
