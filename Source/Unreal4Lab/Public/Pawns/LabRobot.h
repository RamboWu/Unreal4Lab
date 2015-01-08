// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawns/LabPawn.h"
#include "LabRobot.generated.h"

/**
 * 
 */
UCLASS(hideCategories = (Rendering, Input, Actor, Base, Collision, Shape, Transform, Object, Tags, Blueprint))
class UNREAL4LAB_API ALabRobot : public ALabPawn
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree * BotBehavior;
	
};
