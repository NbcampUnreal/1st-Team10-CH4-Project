
#include "AI/UI/HealthBarWidget.h"

void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthBarWidget::SetBarValuePercent(float const value)
{
	HealthProgressBar -> SetPercent(value);
}
