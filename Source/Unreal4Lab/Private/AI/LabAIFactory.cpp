// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "AIController.h"
#include "LabAIController.h"
#include "LabAIFactory.h"


ALabAIFactory::ALabAIFactory(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	SpawnInterval = 30.f;
	SpawnNum = 4;
	TeamNum = ELabTeam::Neutral;
	
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
				GetWorldTimerManager().SetTimer(this, &ALabAIFactory::OnSpawnCharacter, SpawnInterval, true);
			}
		}
	}

}

void ALabAIFactory::OnSpawnCharacter()
{
	if (Role == ROLE_Authority)
	{
		for (uint32 i = 0; i < SpawnNum; i++)
		{
			// Get current location of the Player Proxy
			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			SpawnParams.bNoCollisionFail = true;

			// Spawn the actual player character at the same location as the Proxy
			ACharacter* Character = Cast<ACharacter>(GetWorld()->SpawnActor(CharacterClass, &Location, &Rotation, SpawnParams));

			// We use the PlayerAI to control the Player Character for Navigation
			ALabAIController* PlayerAI = GetWorld()->SpawnActor<ALabAIController>(GetActorLocation(), GetActorRotation());
			PlayerAI->Possess(Character);
		}

	}
}


