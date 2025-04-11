#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h" // 부모 클래스 헤더
#include "CSTypes/CSCharacterTypes.h" // EJobTypes
// 전방 선언으로 변경 (헤더 포함 최소화)
// #include "Components/TextBlock.h"
// #include "Components/Image.h"
// #include "PlayerStates/CSPlayerState.h"
#include "CSPlayerEntry.generated.h"

// 전방 선언
class UTextBlock;
class UImage;
class ACSPlayerState; // TWeakObjectPtr 사용 위해 필요

UCLASS()
class CS_API UCSPlayerEntry : public UCSUIBaseWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Player Entry")
    void SetupPlayerEntry(ACSPlayerState* InPlayerState);
protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Player Entry")
    void UpdateEntryUI();

    // --- 위젯 바인딩 + 블루프린트 접근 허용 ---
    // BlueprintReadOnly 추가!
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    TObjectPtr<UTextBlock> PlayerNameText;

    // BlueprintReadOnly 추가!
    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    TObjectPtr<UTextBlock> ReadyStatusText;

    // BlueprintReadOnly 추가!
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
    TObjectPtr<UTextBlock> JobText;

    // BlueprintReadOnly 추가!
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
    TObjectPtr<UImage> CharacterIcon;
    // ------------------------------------------

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Entry")
    TWeakObjectPtr<ACSPlayerState> AssociatedPlayerState;

    virtual void NativeConstruct() override;
};