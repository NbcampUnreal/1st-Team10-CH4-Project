#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSPlayerEntry.generated.h"

class UTextBlock;
class UImage;
class ACSPlayerState;

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

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    TObjectPtr<UTextBlock> PlayerNameText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    TObjectPtr<UTextBlock> ReadyStatusText;

    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
    TObjectPtr<UTextBlock> JobText;

    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
    TObjectPtr<UImage> CharacterIcon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Entry")
    TWeakObjectPtr<ACSPlayerState> AssociatedPlayerState;

    virtual void NativeConstruct() override;
};