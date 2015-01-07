// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "LabPawnReplicationInfo.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ULabPawnReplicationInfo : public UObject
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	int32 HealthMax;

	// How much health the pawn should regenerate per second
	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	float HealthRegenerationAmount;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	int32 Level;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	int32 Damage;
};
