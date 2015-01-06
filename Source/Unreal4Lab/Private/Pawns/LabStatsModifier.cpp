// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabBuff.h"
#include "LabStatsModifier.h"


ULabStatsModifier::ULabStatsModifier(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


float ULabStatsModifier::CalculateStat(EStatName StatName, float BaseValue)
{
	float NewValue;

	// Different stats calculate in different ways!
	switch (StatName)
	{

	case STATNAME_HPMax:
	case STATNAME_HPRegen:
	case STATNAME_ManaMax:
	case STATNAME_ManaRegen:
	case STATNAME_Damage:

	default:
		NewValue = (BaseValue + CalculateGrowthChanges(StatName)) * CalculateMultipliedChanges(StatName) + CalculateAbsoluteChanges(StatName);
		break;
	}

	/*CalculatePreLockCaps(StatName, NewValue);
	CalculateAssignedChanges(StatName, NewValue);
	CalculateFinalCaps(StatName, NewValue);
	*/
	return NewValue;
}

void ULabStatsModifier::AddStatChange(EStatName StatName, float Modification, EModifierType ModType, float Expiry /*= 0.f*/, bool StaysOnDeath /*= false*/)
{
	ULabBuff* NewBuff;
	FStatChange NewStatChange;

	// Create the stat change
	NewStatChange.StatName = StatName;
	NewStatChange.Modification = Modification;
	NewStatChange.ModificationType = ModType;

	NewBuff = ConstructObject<ULabBuff>(ULabBuff::StaticClass());
	NewBuff->StatChanges.Add(NewStatChange);
	NewBuff->Expiry = Expiry;
	NewBuff->StaysOnDeath = StaysOnDeath;

	// Add the stat change to the array
	AddToBuffs(NewBuff);
}

void ULabStatsModifier::AddToBuffs(class ULabBuff* NewBuff)
{
	int32 i;

	// Attempt to insert the buff into the array; sorting on their Expiry time
	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		if (NewBuff->Expiry < AllBuffs[i]->Expiry)
		{
			AllBuffs.Insert(NewBuff, i);
			return;
		}
	}

	// Otherwise, add it to the end of the array
	AllBuffs.Add(NewBuff);
}

float ULabStatsModifier::CalculateMultipliedChanges(EStatName StatName)
{
	float TotalModification;
	int32 i, j;

	TotalModification = 1.f;

	// Iterate through the stat changes and append to find the total modification

	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == MODTYPE_Multiplication)
			{
				TotalModification *= AllBuffs[i]->StatChanges[j].Modification;
			}
		}
	}


	// Return the total modification
	return TotalModification;
}

float ULabStatsModifier::CalculateAbsoluteChanges(EStatName StatName)
{
	float TotalModification = 0;
	int32 i, j;

	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == MODTYPE_Addition)
			{
				TotalModification += AllBuffs[i]->StatChanges[j].Modification;
			}
		}
	}

	return TotalModification;
}


float ULabStatsModifier::CalculateGrowthChanges(EStatName StatName)
{
	float TotalModification = 0;
	int32 i, j;

	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == MODTYPE_Growth)
			{
				TotalModification += AllBuffs[i]->StatChanges[j].Modification;
			}
		}
	}

	return TotalModification;
}


float ULabStatsModifier::CalculateChangesDiminishingly(EStatName StatName)
{
	float TotalModification;
	int32 i, j;

	TotalModification = 1.f;

	// Iterate through the stat changes and append to find the total modification
	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName)
			{
				TotalModification *= (1.f - AllBuffs[i]->StatChanges[j].Modification);
			}
		}
	}

	// Return the total modification
	return (1.f - TotalModification);
}

float ULabStatsModifier::FindSmallestChange(EStatName StatName, EModifierType ModificationType)
{
	float SmallestModification = MAX_FLT;
	bool FoundSmallest = false;
	int32 i, j;

	// Find the smallest modification
	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == ModificationType)
			{
				if (!FoundSmallest)
				{
					FoundSmallest = true;
					SmallestModification = AllBuffs[i]->StatChanges[j].Modification;
				}
				else if (AllBuffs[i]->StatChanges[j].Modification < SmallestModification)
				{
					SmallestModification = AllBuffs[i]->StatChanges[j].Modification;
				}
			}
		}
	}


	// Return the smallest modification, if found
	if (FoundSmallest)
	{
		return SmallestModification;
	}

	// Return 0.f, if the smallest wasn't found and if the modification type was addition
	if (ModificationType == MODTYPE_Addition || MODTYPE_Growth)
	{
		return 0.f;
	}

	// Otherwise, return 1.f as a fail safe option
	return 1.f;
}

float ULabStatsModifier::FindLargestChange(EStatName StatName, EModifierType ModificationType)
{
	float LargestModification = 0;
	bool FoundLargest = false;
	int32 i, j;

	// Find the largest modification
	for (i = 0; i < AllBuffs.Num(); ++i)
	{
		for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
		{
			if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == ModificationType)
			{
				if (!FoundLargest)
				{
					FoundLargest = true;
					LargestModification = AllBuffs[i]->StatChanges[j].Modification;
				}
				else if (AllBuffs[i]->StatChanges[j].Modification > LargestModification)
				{
					LargestModification = AllBuffs[i]->StatChanges[j].Modification;
				}
			}
		}
	}

	// Return the largest modification if found
	if (FoundLargest)
	{
		return LargestModification;
	}

	// Return 0.f, if the largest wasn't found and if the modification type was addition
	if (ModificationType == MODTYPE_Addition || MODTYPE_Growth)
	{
		return 0.f;
	}

	// Otherwise, return 1.f as a fail safe option
	return 1.f;
}

void ULabStatsModifier::CalculateAssignedChanges(EStatName StatName, float &PreviousValue)
{
	int32 i, j;

	if (AllBuffs.Num() > 0)
	{
		for (i = 0; i < AllBuffs.Num(); ++i)
		{
			for (j = 0; j < AllBuffs[i]->StatChanges.Num(); ++j)
			{
				if (AllBuffs[i]->StatChanges[j].StatName == StatName && AllBuffs[i]->StatChanges[j].ModificationType == MODTYPE_Assignment)
				{
					PreviousValue = AllBuffs[i]->StatChanges[j].Modification;
				}
			}
		}
	}
}

void ULabStatsModifier::CalculatePreLockCaps(EStatName StatName, float &PreviousValue)
{

	switch (StatName)
	{
	case STATNAME_Speed: // can't go below 100 or above 520 in DotA
		PreviousValue = FMath::Clamp(PreviousValue, 120.f, 830.f);
		break;

	case STATNAME_AttackSpeed: // can't go below -80 or above 400
		PreviousValue = FMath::Clamp(PreviousValue, -80.f, 400.f);
		break;

	default:
		break;
	}
}

void ULabStatsModifier::CalculateFinalCaps(EStatName StatName, float &PreviousValue)
{
	switch (StatName)
	{
	case STATNAME_Armor: // can't go below -20
		PreviousValue = FMath::Max(PreviousValue, -20.f);
		break;

	default:
		break;
	}
}

void ULabStatsModifier::RemoveExpired()
{
	int32 i = 0;

	while (i < AllBuffs.Num())
	{
		// Skip as it hasn't expired
		if (AllBuffs[i]->Expiry >= GetWorld()->GetTimeSeconds())
		{
			i++;
		}
		else
			// Remove the entry as it has expired
			AllBuffs.Remove(AllBuffs[i]);
	}
}

void ULabStatsModifier::ResetForDeath()
{
	int32 i = 0;

	while (i < AllBuffs.Num())
	{
		if (!AllBuffs[i]->StaysOnDeath)
			AllBuffs.Remove(AllBuffs[i]);
		else
			i++;
	}
}



