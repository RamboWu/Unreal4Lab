// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Online/LabGameMode.h"
#include "LabGameMode_TEAM_SP.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabGameMode_TEAM_SP : public ALabGameMode
{
	GENERATED_UCLASS_BODY()

	
	/** setup team changes at player login */
	void PostLogin(APlayerController* NewPlayer) override;


protected:
	/** pick team with least players in or random when it's equal */
	int32 ChooseTeam(class ALabPlayerState* ForPlayerState) const;

	/** check team constraints */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** number of teams */
	int32 NumTeams;
};
