// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabAIController.h"
#include "LabRobot.h"


ALabRobot::ALabRobot(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	AIControllerClass = ALabAIController::StaticClass();

}


