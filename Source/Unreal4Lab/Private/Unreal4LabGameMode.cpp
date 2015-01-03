// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Unreal4Lab.h"
#include "Unreal4LabGameMode.h"
#include "Unreal4LabPlayerController.h"
#include "LabPlayerProxy.h"
#include "Unreal4LabCharacter.h"

AUnreal4LabGameMode::AUnreal4LabGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// use our custom PlayerController class
	PlayerControllerClass = AUnreal4LabPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	/*static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/MyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}*/
	DefaultPawnClass = ALabPlayerProxy::StaticClass();
}

bool AUnreal4LabGameMode::OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const ILabTeamInterface* TeamA = InterfaceCast<const ILabTeamInterface>(ActorA);
	const ILabTeamInterface* TeamB = InterfaceCast<const ILabTeamInterface>(ActorB);

	if ((TeamA != NULL && TeamA->GetTeamNum() == ELabTeam::Unknown) || (TeamB != NULL && TeamB->GetTeamNum() == ELabTeam::Unknown))
		return true;

	return (TeamA != NULL) && (TeamB != NULL) && (TeamA->GetTeamNum() == TeamB->GetTeamNum());
}

bool AUnreal4LabGameMode::OnEnemyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const ILabTeamInterface* TeamA = InterfaceCast<const ILabTeamInterface>(ActorA);
	const ILabTeamInterface* TeamB = InterfaceCast<const ILabTeamInterface>(ActorB);

	if ((TeamA != NULL && TeamA->GetTeamNum() == ELabTeam::Unknown) || (TeamB != NULL && TeamB->GetTeamNum() == ELabTeam::Unknown))
		return false;

	return (TeamA != NULL) && (TeamB != NULL) && (TeamA->GetTeamNum() != TeamB->GetTeamNum());
}
