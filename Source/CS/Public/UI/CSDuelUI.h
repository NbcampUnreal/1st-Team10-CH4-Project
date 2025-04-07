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

	// 온라인 대결 모드 전용 UI 업데이트 함수 오버라이드
	virtual void UpdateUI() override;
};
