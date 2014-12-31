// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawn.h"


ALabPawn::ALabPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), m_base_health(100), m_health(100), m_base_attack_range(100), m_base_sight_distance(300)
{
	bReplicates = true;
}

void ALabPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(ALabPawn, m_team_num);
	DOREPLIFETIME(ALabPawn, m_health);
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
	return m_health;
}

float ALabPawn::GetHealthPercentage()
{
	return ((float)m_health / GetMaxHealth());
}

uint32 ALabPawn::GetMaxHealth()
{
	return m_base_health;
}

uint32 ALabPawn::GetAttackRange() const
{
	return m_base_attack_range;
}

void ALabPawn::Client_PlayMeleeAnim()
{

	if ((m_health > 0.f) && MeleeAnim)
	{
		
		float duration = PlayAnimMontage(MeleeAnim);

	}
}

uint32 ALabPawn::GetSightDistance()
{
	return m_base_sight_distance;
}
