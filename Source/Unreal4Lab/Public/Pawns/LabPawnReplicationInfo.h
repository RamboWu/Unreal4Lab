// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "LabPawnReplicationInfo.generated.h"

/** replicated information on a hit we've taken */
USTRUCT()
struct FLabPawnReplicationInfo
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(BlueprintReadOnly, Category = State)
	int32 HealthMax;

	UPROPERTY(BlueprintReadOnly, Category = State)
	int32 ManaMax;
	
	// How much health the pawn should regenerate per second
	UPROPERTY(BlueprintReadOnly, Category = State)
	float HealthRegenerationAmount;

	UPROPERTY(BlueprintReadOnly, Category = State)
	float ManaRegenerationAmount;

	UPROPERTY(BlueprintReadOnly, Category = State)
	int32 Level;

	UPROPERTY(BlueprintReadOnly, Category = State)
	int32 Damage;

	UPROPERTY(BlueprintReadOnly, Category = Spell)
	TArray<class ALabSpell*> Spells;

public:
	FLabPawnReplicationInfo()
	{}
};
