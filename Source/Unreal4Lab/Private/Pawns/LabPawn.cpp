// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabBlueprintLibrary.h"
#include "LabPawnReplicationInfo.h"
#include "LabStatsModifier.h"
#include "LabPawn.h"


ALabPawn::ALabPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), 
	BaseHealth(100),
	Health(0),
	m_base_attack_range(100), 
	m_base_sight_distance(300)
{
	bReplicates = true;
	JustSpawned = true;
	
}

void ALabPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALabPawn, m_team_num);
	DOREPLIFETIME(ALabPawn, Health);
	DOREPLIFETIME(ALabPawn, PawnReplicationInfo);
}

uint8 ALabPawn::GetTeamNum() const
{
	return m_team_num;
}

void ALabPawn::SetTeamNum(uint8 NewTeamNum)
{
	m_team_num = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}

uint32 ALabPawn::GetHealth()
{
	return Health;
}

float ALabPawn::GetHealthPercentage()
{
	return ((float)Health / GetMaxHealth());
}

uint32 ALabPawn::GetMaxHealth()
{
	return BaseHealth;
}

uint32 ALabPawn::GetAttackRange() const
{
	return m_base_attack_range;
}

void ALabPawn::Client_PlayMeleeAnim_Implementation()
{

	if ((Health > 0.f) && MeleeAnim)
	{
		float duration = PlayAnimMontage(MeleeAnim);
	}
}

uint32 ALabPawn::GetSightDistance()
{
	return m_base_sight_distance;
}


void ALabPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		// Recalculates the pawn stats
		RecalculateStats();

		// Update the health and mana
		if (PawnReplicationInfo != NULL)
		{
			Health = FMath::Min(float(PawnReplicationInfo->HealthMax), Health + (PawnReplicationInfo->HealthRegenerationAmount * DeltaTime));
			//Mana = FMin(UDKMOBAPawnReplicationInfo.ManaMax, Mana + (UDKMOBAPawnReplicationInfo.ManaRegenerationAmount * DeltaTime));
		}
	}

}

void ALabPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		PawnReplicationInfo = ConstructObject<ULabPawnReplicationInfo>(ULabPawnReplicationInfo::StaticClass(), this);
		StatsModifier = ConstructObject<ULabStatsModifier>(ULabStatsModifier::StaticClass(), this);
	}
}


void ALabPawn::RecalculateStats()
{
	if (!HasAuthority())
		return;

	if (StatsModifier == NULL)
	{
		return;
	}

	if (PawnReplicationInfo != NULL)
	{
		// MUST calc attributes first
		//UDKMOBAPawnReplicationInfo.Strength = StatsModifier.CalculateStat(STATNAME_Strength);
		//UDKMOBAPawnReplicationInfo.Agility = StatsModifier.CalculateStat(STATNAME_Agility);
		//UDKMOBAPawnReplicationInfo.Intelligence = StatsModifier.CalculateStat(STATNAME_Intelligence);

		// Now can go through the rest
		//UDKMOBAPawnReplicationInfo.ManaMax = StatsModifier.CalculateStat(STATNAME_ManaMax, BaseMana);

		PawnReplicationInfo->HealthMax = StatsModifier->CalculateStat(STATNAME_HPMax, BaseHealth);
		// If just spawned, then set Mana  and Health to Max
		if (JustSpawned)
		{
			JustSpawned = false;
			Health = PawnReplicationInfo->HealthMax;
			//ULabBlueprintLibrary::printDebugInfo("Set "+ GetName() + "'s health = "+FString::FromInt(Health));
		}
	}
}