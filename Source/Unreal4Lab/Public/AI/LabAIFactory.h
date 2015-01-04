// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LabTypes.h"
#include "GameFramework/Actor.h"
#include "LabAIFactory.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabAIFactory : public AActor
{
	GENERATED_UCLASS_BODY()

	// Pawn archetype to spawn
	UPROPERTY(EditAnywhere, Category = AIFactory)
	TSubclassOf<AActor> CharacterClass;

	// Time between each spawn
	UPROPERTY(EditAnywhere, Category = AIFactory)
	float SpawnInterval;

	UPROPERTY(EditAnywhere, Category = AIFactory)
	uint32 SpawnNum;

	UPROPERTY(EditAnywhere, Category = AIFactory)
	class ALabAIRoute* Route;

	UPROPERTY(EditAnywhere, Category = AIFactory)
	TEnumAsByte<ELabTeam::Type> TeamNum;


	virtual void BeginPlay() override;

protected:
	void OnSpawnCharacter();
};
