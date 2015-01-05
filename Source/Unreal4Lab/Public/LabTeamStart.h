// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LabTypes.h"
#include "GameFramework/PlayerStart.h"
#include "LabTeamStart.generated.h"

/**
 * 
 */
UCLASS(hideCategories = (Rendering, Input, Actor, Base, Collision, Shape, Transform, Object, Tags, Blueprint))
class UNREAL4LAB_API ALabTeamStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()

	/** Which team can start at this point */
	UPROPERTY(EditInstanceOnly, Category = Team)
	TEnumAsByte<ELabTeam::Type> SpawnTeam;
};
