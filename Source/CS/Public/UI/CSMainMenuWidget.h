// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBase.h"
#include "Components/Button.h"
#include "CSMainMenuWidget.generated.h"


UCLASS()
class CS_API UCSMainMenuWidget : public UCSUIBase
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* SoloModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OnlineDuelModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OnlineCoopModeButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OptionsButton;

    // ��ư Ŭ�� �̺�Ʈ �ڵ鷯
    UFUNCTION()
    void HandleSoloModeClicked();

    UFUNCTION()
    void HandleOnlineDuelClicked();

    UFUNCTION()
    void HandleOnlineCoopClicked();

    UFUNCTION()
    void HandleOptionsClicked();
};
