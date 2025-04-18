// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSTypes/CSCharacterTypes.h"
#include "Engine/DataTable.h"
#include "CSLobbyCharacter.generated.h"

UCLASS()
class CS_API ACSLobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACSLobbyCharacter();

	UFUNCTION(BlueprintCallable, Category = "Lobby Character")
	void UpdateMeshFromJobType(EJobTypes JobType);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateMesh(EJobTypes NewJob);
	void Multicast_UpdateMesh_Implementation(EJobTypes NewJob);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class UDataTable* CharacterLobbyDataTable;

};
