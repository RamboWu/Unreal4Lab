// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Unreal4Lab.h"
#include "LabGameMode.h"
#include "Unreal4LabPlayerController.h"
#include "LabPlayerProxy.h"
#include "Unreal4LabCharacter.h"

ALabGameMode::ALabGameMode(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	// use our custom PlayerController class
	PlayerControllerClass = AUnreal4LabPlayerController::StaticClass();

	DefaultPawnClass = ALabPlayerProxy::StaticClass();
}

bool ALabGameMode::OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const ILabTeamInterface* TeamA = InterfaceCast<const ILabTeamInterface>(ActorA);
	const ILabTeamInterface* TeamB = InterfaceCast<const ILabTeamInterface>(ActorB);

	if ((TeamA != NULL && TeamA->GetTeamNum() == ELabTeam::Unknown) || (TeamB != NULL && TeamB->GetTeamNum() == ELabTeam::Unknown))
		return true;

	return (TeamA != NULL) && (TeamB != NULL) && (TeamA->GetTeamNum() == TeamB->GetTeamNum());
}

bool ALabGameMode::OnEnemyTeam(const AActor* ActorA, const AActor* ActorB)
{
	const ILabTeamInterface* TeamA = InterfaceCast<const ILabTeamInterface>(ActorA);
	const ILabTeamInterface* TeamB = InterfaceCast<const ILabTeamInterface>(ActorB);

	if ((TeamA != NULL && TeamA->GetTeamNum() == ELabTeam::Unknown) || (TeamB != NULL && TeamB->GetTeamNum() == ELabTeam::Unknown))
		return false;

	return (TeamA != NULL) && (TeamB != NULL) && (TeamA->GetTeamNum() != TeamB->GetTeamNum());
}

void ALabGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void ALabGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

bool ALabGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

AActor* ALabGameMode::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	APlayerStart* BestStart = NULL;
	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestSpawn = PlayerStarts[i];
		if (Cast<APlayerStartPIE>(TestSpawn) != NULL)
		{
			// Always prefer the first "Play from Here" PlayerStart, if we find one while in PIE mode
			BestStart = TestSpawn;
			break;
		}
		else if (TestSpawn != NULL)
		{
			if (IsSpawnpointAllowed(TestSpawn, Player))
			{
				if (IsSpawnpointPreferred(TestSpawn, Player))
				{
					PreferredSpawns.Add(TestSpawn);
				}
				else
				{
					FallbackSpawns.Add(TestSpawn);
				}
			}
		}
	}


	if (BestStart == NULL)
	{
		if (PreferredSpawns.Num() > 0)
		{
			BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
		}
		else if (FallbackSpawns.Num() > 0)
		{
			BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
		}
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}


bool ALabGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	return true;
}

bool ALabGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Player ? Cast<ACharacter>(Player->GetPawn()) : NULL;
	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->CapsuleComponent->GetScaledCapsuleHalfHeight() + OtherPawn->CapsuleComponent->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->CapsuleComponent->GetScaledCapsuleRadius() + OtherPawn->CapsuleComponent->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}

	return true;
}