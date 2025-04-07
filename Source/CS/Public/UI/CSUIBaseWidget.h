// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSUIBaseWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CS_API UCSUIBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    virtual void UpdateUI();
};
