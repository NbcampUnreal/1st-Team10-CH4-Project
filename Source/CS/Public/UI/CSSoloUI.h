// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBase.h"
#include "CSSoloUI.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UCSSoloUI : public UCSUIBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 솔로 모드 전용 UI 업데이트 (필요 시 오버라이드)
	virtual void UpdateUI() override;
};
