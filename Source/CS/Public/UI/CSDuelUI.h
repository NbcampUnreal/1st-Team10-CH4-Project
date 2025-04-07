// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBase.h"
#include "CSDuelUI.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UCSDuelUI : public UCSUIBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// �¶��� ��� ��� ���� UI ������Ʈ �Լ� �������̵�
	virtual void UpdateUI() override;
};
