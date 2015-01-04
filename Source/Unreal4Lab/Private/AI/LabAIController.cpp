// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "LabRobot.h"
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
}

void ALabAIController::SetEnemy(class APawn *InPawn)
{
	if (InPawn)
	{
		BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);
		BlackboardComp->SetValueAsVector(EnemyLocationID, InPawn->GetActorLocation());
	}
	else
	{

		BlackboardComp->SetValueAsObject(EnemyKeyID, NULL);
		BlackboardComp->ClearValueAsVector(EnemyLocationID);
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
	if (enemey)
	{
		if (AllTargets.Contains(Cast<AActor>(enemey)))
		{
			return;
		}
	}
	SetEnemy(NULL);

	const FVector my_location = my_bot->GetActorLocation();
	float best_dist = MAX_FLT;
	ALabPawn* best_target = NULL;

	for (int32 i = 0; i < AllTargets.Num(); i++)
	{
		APawn* tmp_target1 = Cast<APawn>(AllTargets[i]);
		if (tmp_target1 && my_bot == tmp_target1)
		{
			continue;
		}

		ALabPawn* tmp_target = Cast<ALabPawn>(AllTargets[i]);
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
				self_pawn->Client_PlayMeleeAnim();
				MeleeAttackAnimationEndTime = GetWorld()->GetTimeSeconds() + 1.f;
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
		bMovingToTarget = false;
		if (PathFollowingComponent.IsValid())
		{
			PathFollowingComponent->AbortMove(TEXT("close enought"));
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
			ULabBlueprintLibrary::printDebugInfo("add a target:" + OtherActor->GetName());
			AllTargets.AddUnique(OtherActor);
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
			ULabBlueprintLibrary::printDebugInfo("remove a target:" + OtherActor->GetName());
			AllTargets.Remove(OtherActor);
		}
	}
}
