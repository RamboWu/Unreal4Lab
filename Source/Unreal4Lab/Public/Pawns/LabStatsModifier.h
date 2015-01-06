// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "LabStatsModifier.generated.h"

// Modifier type
UENUM(BlueprintType)
enum EModifierType
{
	MODTYPE_Growth			UMETA(DisplayName = "Growth"),	//这是随着等级成长获得到的属性增长
	MODTYPE_Multiplication	UMETA(DisplayName = "Multiplication"),
	MODTYPE_Addition		UMETA(DisplayName = "Addition"),
	MODTYPE_Assignment		UMETA(DisplayName = "Assign(Lock)"),
};

// State name
UENUM(BlueprintType)
enum EStatName
{

	STATNAME_HPMax			UMETA(DisplayName = "Hit Points Maximum"), // float 0+
	STATNAME_HPRegen		UMETA(DisplayName = "Hit Points Regeneration"), // float
	STATNAME_ManaMax		UMETA(DisplayName = "Mana Maximum"), // float 0+
	STATNAME_ManaRegen		UMETA(DisplayName = "Mana Regeneration"), // float
	STATNAME_Damage			UMETA(DisplayName = "Damage"), // float 0+
	STATNAME_Speed			UMETA(DisplayName = "Speed"), // int 0+
	STATNAME_AttackSpeed    UMETA(DisplayName = "Attack Speed"), // float 0+ %
	STATNAME_Armor			UMETA(DisplayName = "Armor")// float
};

// Stat change struct
USTRUCT()
struct FStatChange
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, Category = StatChange)
	TEnumAsByte<EStatName> StatName;

	UPROPERTY(VisibleAnywhere, Category = StatChange)
	float Modification;

	UPROPERTY(VisibleAnywhere, Category = StatChange)
	TEnumAsByte<EModifierType> ModificationType;
};

UCLASS()
class UNREAL4LAB_API ULabStatsModifier : public UObject
{
	GENERATED_UCLASS_BODY()

	// Array of all active buffs on that we're managing
	TArray<class ULabBuff*> AllBuffs;
	
	/**
	* Calculate all the changes that effect a stat, and return the result
	*/
	float CalculateStat(EStatName StatName, float BaseValue = 0.f);
	
	/**
	* Create a stat change for the given stat, optionally with an expiry time. Add this with a shell buff.
	*/
	void AddStatChange(EStatName StatName, float Modification, EModifierType ModType, float Expiry = 0.f, bool StaysOnDeath = false);

	/**
	* Add given buff to the list, so it gets bundled into all our calculations
	*/
	void AddToBuffs(class ULabBuff* NewBuff);

	/**
	* Remove old buffs from the changes list
	*/
	void RemoveExpired();

	/*Remove all buffs that don't carry across on death from the changes list
	*/
	void ResetForDeath();

protected:
	/**
	* Find multiplicative changes, and multiply them together
	*/
	float CalculateMultipliedChanges(EStatName StatName);

	/**
	* Find absolute changes, and add them together
	*/
	float CalculateAbsoluteChanges(EStatName StatName);

	/**
	* Find absolute changes, and add them together
	*/
	float CalculateGrowthChanges(EStatName StatName);
	
	/**
	* Find changes, and multiply them together in a diminishing fashion - multiplying them as
	* percentages. This makes 0.5 and 0.5 combine to form 0.75.
	*/
	float CalculateChangesDiminishingly(EStatName StatName);
	/**
	* Find changes of the given type, and return the smallest only (used for some non-stacking stats
	* like slows). Only really makes sense for additive and multiplicative changes.
	*/
	float FindSmallestChange(EStatName StatName, EModifierType ModificationType);

	/**
	* Find changes of the given type, and return the largest only (used for some non-stacking stats
	* like evasion). Only really makes sense for additive and multiplicative changes.
	*/
	float FindLargestChange(EStatName StatName, EModifierType ModificationType);

	/**
	* Find assigned (locked) changes, and apply them
	*/
	void CalculateAssignedChanges(EStatName StatName, float &PreviousValue);

	/**
	* Apply any caps to the range of a stat before locks (assignments) - caps stats within a sensible range
	*/
	void CalculatePreLockCaps(EStatName StatName, float &PreviousValue);

	/**
	* Apply any caps to the range of a stat AFTER locks (assignments) - stops stats from being invalid values.
	*/
	void CalculateFinalCaps(EStatName StatName, float &PreviousValue);
};
