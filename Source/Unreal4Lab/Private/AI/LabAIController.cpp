// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "LabPawnReplicationInfo.h"
#include "LabRobot.h"
#include "LabGameMode.h"
#include "LabAIRoute.h"
#include "LabAIController.h"
#include "LabBlueprintLibrary.h"


ALabAIController::ALabAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), 
	MeleeAttackAnimationEndTime(0),
	bIsPlayingAnimation(false), 
	bMovingToTarget(false)
{
	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	SensingComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SensingComponent"));
	SensingComponent->OnComponentBeginOverlap.AddDynamic(this, &ALabAIController::OnBeginOverlap);
	SensingComponent->OnComponentEndOverlap.AddDynamic(this, &ALabAIController::OnEndOverlap);

	Route = NULL;
	//SensingComponent->InitSphereRadius(300);
	//SensingComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SensingComponent->AttachTo(RootComponent);
}

void ALabAIController::Possess(class APawn *InPawn)
{
	Super::Possess(InPawn);
	ALabRobot* Bot = Cast<ALabRobot>(InPawn);
	if (Bot && Bot->BotBehavior)
	{
		BlackboardComp->InitializeBlackboard(Bot->BotBehavior->BlackboardAsset);

		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		EnemyLocationID = BlackboardComp->GetKeyID("Destination");

		BehaviorComp->StartTree(Bot->BotBehavior);
	}

	ILabStateInterface* state = InterfaceCast<ILabStateInterface>(InPawn);
	if (state)
	{
		SensingComponent->SetSphereRadius(state->GetSightDistance());
		SensingComponent->AttachTo(InPawn->GetRootComponent());
	}

	if (Route != NULL)
	{
		cur_route_node_index = 0;
		SetDestination(cur_route_node_index++);
	}
}

void ALabAIController::SetEnemy(class APawn *InPawn)
{
	if (InPawn)
	{
		BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);
	}
	else
	{
		BlackboardComp->SetValueAsObject(EnemyKeyID, NULL);
	}
	
}


void ALabAIController::SetDestination(int32 index)
{
	if (Route)
	{
		if (index < Route->all_targets.Num())
		{
			
			BlackboardComp->SetValueAsVector(EnemyLocationID, Route->all_targets[index]->GetActorLocation());
		}
		
	}
	
}


void ALabAIController::SearchForEnemy()
{
	APawn* my_bot = GetPawn();
	if (my_bot == NULL)
	{
		return;
	}

	APawn* enemey = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));

	//check if the current enemy is still alive and in the sight distance
	//TODO IsValid 判断是否死亡还不是很准确
	if (enemey && IsTargetValid(enemey))
	{
		if (AllTargets.Contains(Cast<AActor>(enemey)))
		{
			//ULabBlueprintLibrary::printDebugInfo("Current enemy" + enemey->GetName() + "Is still valid!");
			return;
		}
	}
	SetEnemy(NULL);

	const FVector my_location = my_bot->GetActorLocation();
	float best_dist = MAX_FLT;
	ALabPawn* best_target = NULL;


	int32 i = 0;
	while (i < AllTargets.Num())
	{
		//check if it is self
		APawn* tmp_target1 = Cast<APawn>(AllTargets[i]);
		if (tmp_target1 && my_bot == tmp_target1)
		{
			i++;
			continue;
		}
		ALabPawn* tmp_target = Cast<ALabPawn>(AllTargets[i]);
		if (tmp_target)
		{
			if (!IsTargetValid(tmp_target))
			{
				//ULabBlueprintLibrary::printDebugInfo(tmp_target->GetName() + "Is not valid target any more!");
				AllTargets.RemoveAt(i);
				continue;
			}
			//ULabBlueprintLibrary::printDebugInfo(tmp_target->GetName() + "Is still valid!");
			const float dist = FVector::Dist(tmp_target->GetActorLocation(), my_location);
			if (dist < best_dist)
			{
				best_dist = dist;
				best_target = tmp_target;
			}
			i++;
		}
	}

	if (best_target)
	{
		SetEnemy(best_target);
	}
}



bool ALabAIController::withInAttackRange()
{
	APawn* enemey = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));
	ALabPawn* self_pawn = Cast<ALabPawn>(GetPawn());
	if (enemey && self_pawn)
	{
		FVector length_vec = enemey->GetActorLocation() - self_pawn->GetActorLocation();
		float dist = length_vec.Size();
		if (dist <= self_pawn->GetAttackRange())
		{
			return true;
		}
		else
			return false;

	}
	else
		return false;
}

void ALabAIController::Attack()
{
	if (HasAuthority()){
		ALabPawn* self_pawn = Cast<ALabPawn>(GetPawn());
		APawn* enemey = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));

		bIsPlayingAnimation = GetWorld()->GetTimeSeconds() < MeleeAttackAnimationEndTime;


		if (self_pawn && !bIsPlayingAnimation && enemey)
		{

			// try move closer if needed again
			MoveCloser();

			if (!bMovingToTarget)
			{
				float duration = self_pawn->ServerPlayAttackMontage();

				FDamageEvent damage_event;
				enemey->TakeDamage(self_pawn->PawnReplicationInfo->Damage, damage_event, this, self_pawn);
				MeleeAttackAnimationEndTime = GetWorld()->GetTimeSeconds() + duration;
				bIsPlayingAnimation = true;
			}

		}
	}

}

void ALabAIController::MoveCloser()
{

	if (bIsPlayingAnimation || bMovingToTarget)
	{
		return;
	}

	APawn* enemey = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));
	ALabPawn* self_pawn = Cast<ALabPawn>(GetPawn());
	if (enemey && self_pawn)
	{
		const float AttackDistance = self_pawn->GetAttackRange();
		FVector length_vec = enemey->GetActorLocation() - self_pawn->GetActorLocation();
		float dist = length_vec.Size();

		if (dist > AttackDistance)
		{
			bMovingToTarget = true;
			EPathFollowingRequestResult::Type  request_result = MoveToActor(enemey, 0.9 * AttackDistance);
		}
	}

}

void ALabAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result != EPathFollowingResult::Skipped)
	{
		if (bMovingToTarget)
		{
			bMovingToTarget = false;
			if (PathFollowingComponent.IsValid())
			{
				PathFollowingComponent->AbortMove(TEXT("close enought"));
			}
		}
		else
		{
			SetDestination(cur_route_node_index++);
		}
		
	}
}

void ALabAIController::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ILabTeamInterface* team_interface = InterfaceCast<ILabTeamInterface>(OtherActor);
	AActor* self_actor = Cast<AActor>(GetPawn());
	if (team_interface && self_actor != NULL && self_actor != OtherActor){
		ALabPawn* target = Cast<ALabPawn>(OtherActor);
		if (target)
		{
			
			if (ALabGameMode::OnEnemyTeam(self_actor, OtherActor))
			{
				//ULabBlueprintLibrary::printDebugInfo("add a target:" + OtherActor->GetName());
				AllTargets.AddUnique(OtherActor);
			}
		}
		
	}
}

void ALabAIController::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	ILabTeamInterface* team_interface = InterfaceCast<ILabTeamInterface>(OtherActor);
	AActor* self_actor = Cast<AActor>(GetPawn());
	if (team_interface && self_actor != NULL && self_actor != OtherActor){
		ALabPawn* target = Cast<ALabPawn>(OtherActor);
		if (target)
		{
			
			if (ALabGameMode::OnEnemyTeam(self_actor, OtherActor))
			{
				//ULabBlueprintLibrary::printDebugInfo("remove a target:" + OtherActor->GetName());
				AllTargets.Remove(OtherActor);
			}

		}
	}
}

bool ALabAIController::IsTargetValid(AActor* InActor) const
{
	// try to find a character
	const ALabPawn* TestChar = Cast<ALabPawn>(InActor);
	if (TestChar == NULL)
	{
		// if Actor is a Controller, try to use its pawn
		const ALabAIController* const Controller = Cast<ALabAIController>(InActor);
		if (Controller != NULL)
		{
			TestChar = Cast<ALabPawn>(Controller->GetPawn());
		}
	}

	
	if (TestChar && ((float)TestChar->GetHealth() > 0.01f) && ALabGameMode::OnEnemyTeam(TestChar, this))
	{
		//ULabBlueprintLibrary::printDebugInfo("TestChar = " + TestChar->GetName() + "health=" + FString::FromInt(TestChar->GetHealth()));
		return true;
	}

	return false;
}