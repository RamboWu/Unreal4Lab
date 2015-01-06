// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "LabStatsModifier.generated.h"

// Modifier type
UENUM(BlueprintType)
enum EModifierType
{
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
	STATNAME_Damage			UMETA(DisplayName = "Damage") // float 0+
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
	//TArray<class UDKMOBABuff> AllBuffs;
	
};
