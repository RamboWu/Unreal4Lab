// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabAIRoute.h"


ALabAIRoute::ALabAIRoute(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Mark = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Mark"));
	Mark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mark->SetHiddenInGame(true);

	RootComponent = Mark;
	
}


