// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSAttributeComponent.generated.h"

class ACSPlayerController;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CS_API UCSAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCSAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDeath();

private:

	UPROPERTY()
	TWeakObjectPtr<ACSPlayerController> OwningPlayerController;

	UPROPERTY(EditAnywhere, Replicated, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Replicated, Category = "Actor Attributes")
	float MaxHealth;

	UFUNCTION()
	void OnRep_Health();

public:
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ReceiveDamage(float DamageAmount, AController* EventInstigator, AActor* DamageCauser, ELaunchTypes DType, FHitResult HitResult);
	void CharacterLaunch(AActor* DamageCauser);

	bool IsAlive();

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }
};
