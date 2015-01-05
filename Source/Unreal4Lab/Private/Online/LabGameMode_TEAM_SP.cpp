// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPlayerState.h"
#include "LabTeamStart.h"
#include "LabGameMode_TEAM_SP.h"


ALabGameMode_TEAM_SP::ALabGameMode_TEAM_SP(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	NumTeams = 2;
}

void ALabGameMode_TEAM_SP::PostLogin(APlayerController* NewPlayer)
{
	// Place player on a team before Super (VoIP team based init, findplayerstart, etc)
	ALabPlayerState* NewPlayerState = CastChecked<ALabPlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);

	Super::PostLogin(NewPlayer);
}

int32 ALabGameMode_TEAM_SP::ChooseTeam(ALabPlayerState* ForPlayerState) const
{
	int32 JusticeNum = 0;
	int32 EvilNum = 0;

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ALabPlayerState const* const TestPlayerState = Cast<ALabPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState) 
		{
			if ((TEnumAsByte<ELabTeam::Type>)TestPlayerState->GetTeamNum() == ELabTeam::Justice)
				JusticeNum++;
			else
				EvilNum++;
		}

	}
	if (JusticeNum <= EvilNum)
		return ELabTeam::Justice;
	else
		return ELabTeam::Evil;
}

bool ALabGameMode_TEAM_SP::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		ALabTeamStart* TeamStart = Cast<ALabTeamStart>(SpawnPoint);
		ALabPlayerState* PlayerState = Cast<ALabPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
		{
			return false;
		}
	}

	return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}

