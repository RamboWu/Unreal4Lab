// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawn.h"


ALabPawn::ALabPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bReplicates = true;
}

void ALabPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	DOREPLIFETIME(ALabPawn, m_team_num);
}

uint8 ALabPawn::GetTeamNum() const
{
	return m_team_num;
}

void ALabPawn::SetTeamNum(uint8 NewTeamNum)
{
	m_team_num = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}