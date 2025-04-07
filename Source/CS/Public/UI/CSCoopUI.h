// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBase.h"
#include "CSCoopUI.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UCSCoopUI : public UCSUIBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// ���� ��� ���� UI ������Ʈ (�ʿ� �� �������̵�)
	virtual void UpdateUI() override;
};
