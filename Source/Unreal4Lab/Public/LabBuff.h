// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "LabStatsModifier.h"
#include "LabBuff.generated.h"

/**
 * 
 */
UCLASS(hidecategories = (Object))
class UNREAL4LAB_API ULabBuff : public UObject
{
	GENERATED_UCLASS_BODY()

	// Stat changes
	UPROPERTY(BlueprintReadOnly, Category = Buff)
	TArray<FStatChange> StatChanges;
	
	// The name of the icon to display with any pawn effected by this buff - if any.
	UPROPERTY(BlueprintReadOnly, Category = Buff)
	FString BuffIcon;

	// How long the buff will last (in seconds)
	UPROPERTY(BlueprintReadOnly, Category = Buff)
	float Expiry;
	
	// Whether this buff is removed when the target dies (true = not removed)
	UPROPERTY(BlueprintReadOnly, Category = Buff)
	uint32 StaysOnDeath:1;

	//TODO
	// Spell (or item) that gave this buff.
	//var UDKMOBASpell BuffOwner;
	
};
