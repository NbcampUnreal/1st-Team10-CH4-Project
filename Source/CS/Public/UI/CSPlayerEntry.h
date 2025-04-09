#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h" // 부모 변경
#include "CSTypes/CSCharacterTypes.h" // EJobTypes
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "PlayerStates/CSPlayerState.h"
#include "CSPlayerEntry.generated.h"

class UTextBlock; class UImage;

UCLASS()
class CS_API UCSPlayerEntry : public UCSUIBaseWidget // 부모 변경
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Player Entry")
    void SetupPlayerEntry(class ACSPlayerState* InPlayerState);
protected:
    // PlayerState 변경 시 호출될 이벤트 (BP 구현)
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Entry")
    void UpdateEntryUI(); // 함수 이름 명확하게 변경

    UPROPERTY(meta = (BindWidget)) UTextBlock* PlayerNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* ReadyStatusText;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* JobText; // 직업 텍스트
    UPROPERTY(meta = (BindWidgetOptional)) UImage* CharacterIcon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Entry")
    TWeakObjectPtr<ACSPlayerState> AssociatedPlayerState;

    virtual void NativeConstruct() override;
};