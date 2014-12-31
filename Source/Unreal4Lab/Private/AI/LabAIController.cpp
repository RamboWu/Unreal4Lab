// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "LabRobot.h"
#include "LabAIController.h"


ALabAIController::ALabAIController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), 
	MeleeAttackAnimationEndTime(0),
	bIsPlayingAnimation(false), 
	bMovingToTarget(false)
{
	BlackboardComp = PCIP.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	BehaviorComp = PCIP.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));


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
}

void ALabAIController::SetEnemy(class APawn *InPawn)
{
	BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);
	BlackboardComp->SetValueAsVector(EnemyLocationID, InPawn->GetActorLocation());
}

void ALabAIController::SearchForEnemy()
{
	APawn* my_bot = GetPawn();
	if (my_bot == NULL)
	{
		return;
	}

	const FVector my_location = my_bot->GetActorLocation();
	float best_dist = MAX_FLT;
	ALabPawn* best_target = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* tmp_target1 = Cast<APawn>(*It);
		if (tmp_target1 && my_bot == tmp_target1)
		{
			continue;
		}

		ALabPawn* tmp_target = Cast<ALabPawn>(*It);
		if (tmp_target)
		{
			const float dist = FVector::Dist(tmp_target->GetActorLocation(), my_location);
			if (dist < best_dist)
			{
				best_dist = dist;
				best_target = tmp_target;
			}
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
	ALabPawn* self_pawn = Cast<ALabPawn>(GetPawn());
	APawn* enemey = Cast<APawn>(BlackboardComp->GetValueAsObject(EnemyKeyID));

	bIsPlayingAnimation = GetWorld()->GetTimeSeconds() < MeleeAttackAnimationEndTime;


	if (self_pawn && !bIsPlayingAnimation && enemey)
	{

		// try move closer if needed again
		MoveCloser();

		if (!bMovingToTarget)
		{
			self_pawn->Client_PlayMeleeAnim();
			MeleeAttackAnimationEndTime = GetWorld()->GetTimeSeconds() + 1.f;
			bIsPlayingAnimation = true;
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
			MoveToActor(enemey, 0.9 * AttackDistance);	
		}
	}

}

void ALabAIController::OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (Result != EPathFollowingResult::Skipped)
	{
		bMovingToTarget = false;
		if (PathFollowingComponent.IsValid())
		{
			PathFollowingComponent->AbortMove(TEXT("close enought"));
		}
	}
}
