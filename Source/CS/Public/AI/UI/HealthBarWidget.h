#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "HealthBarWidget.generated.h"


UCLASS()
class CS_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	void SetBarValuePercent(float const value);
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BlueprintWidget))
	UProgressBar* HealthBar = nullptr;
};
