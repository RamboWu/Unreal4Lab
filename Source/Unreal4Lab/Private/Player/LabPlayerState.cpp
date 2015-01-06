// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPlayerState.h"


ALabPlayerState::ALabPlayerState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void ALabPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALabPlayerState, TeamNumber);
	DOREPLIFETIME(ALabPlayerState, NumKills);
	DOREPLIFETIME(ALabPlayerState, NumDeaths);
}

uint8 ALabPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

void ALabPlayerState::SetTeamNum(uint8 NewTeamNum)
{
	TeamNumber = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}


