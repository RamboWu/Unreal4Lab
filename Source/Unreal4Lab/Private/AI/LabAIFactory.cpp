// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawn.h"
#include "AIController.h"
#include "LabAIController.h"
#include "LabAIFactory.h"
#include "LabBlueprintLibrary.h"

ALabAIFactory::ALabAIFactory(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnInterval = 30.f;
	SpawnNum = 4;
	TeamNum = ELabTeam::Neutral;
	left_spawn_num = 0;

	PrimaryActorTick.bCanEverTick = true;
}

void ALabAIFactory::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		if (CharacterClass != NULL)
		{
			if (SpawnInterval > 0.f)
			{
				
				GetWorldTimerManager().SetTimer(this, &ALabAIFactory::OnNeedToSpawn, SpawnInterval, true);
			}
		}
	}

}

void ALabAIFactory::OnSpawnCharacter()
{
	if (Role == ROLE_Authority)
	{
		//ULabBlueprintLibrary::printDebugInfo("OnSpawnCharacter");

		left_spawn_num--;

		// Get current location of the Player Proxy
		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();

		// Spawn the actual player character at the same location as the Proxy
		ALabPawn* Character = Cast<ALabPawn>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation));

		if (Character)
		{
			Character->SetTeamNum(TeamNum);

			// We use the PlayerAI to control the Player Character for Navigation
			ALabAIController* PlayerAI = GetWorld()->SpawnActor<ALabAIController>(GetActorLocation(), GetActorRotation());
			PlayerAI->Route = Route;
			PlayerAI->Possess(Character);
		}
		

		next_spawn_time = GetWorld()->GetTimeSeconds() + 0.5f;
	}
}


void ALabAIFactory::OnNeedToSpawn()
{
	left_spawn_num += SpawnNum;	
	next_spawn_time = GetWorld()->GetTimeSeconds();
	//ULabBlueprintLibrary::printDebugInfo("OnNeedToSpawn" + FString::FromInt(left_spawn_num) + " Time=" + FString::SanitizeFloat(next_spawn_time));
}


void ALabAIFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (left_spawn_num > 0 && GetWorld()->GetTimeSeconds() > next_spawn_time)
	{
		OnSpawnCharacter();
	}
}


