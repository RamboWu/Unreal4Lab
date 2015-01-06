// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "LabTeamInterface.h"
#include "LabTypes.h"
#include "LabPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabPlayerState : public APlayerState, public ILabTeamInterface
{
	GENERATED_UCLASS_BODY()

	virtual uint8 GetTeamNum() const;

	void SetTeamNum(uint8 NewTeamNum);
	
protected:
	/** team number */
	UPROPERTY(Transient, Replicated)
	TEnumAsByte<ELabTeam::Type> TeamNumber;

	/** number of kills */
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	/** number of deaths */
	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;
};
