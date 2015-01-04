// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabBlueprintLibrary.h"
#include "LabPawn.h"


ALabPawn::ALabPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), m_base_health(100), m_health(100), m_base_attack_range(100), m_base_sight_distance(300)
{
	bReplicates = true;

	//SensingComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SensingComponent"));
	//SensingComponent->OnComponentBeginOverlap.AddDynamic(this, &ALabPawn::OnBeginOverlap);
	//SensingComponent->OnComponentEndOverlap.AddDynamic(this, &ALabPawn::OnEndOverlap);
	//SensingComponent->AttachTo(RootComponent);

}

void ALabPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALabPawn, m_team_num);
	DOREPLIFETIME(ALabPawn, m_health);
}

uint8 ALabPawn::GetTeamNum() const
{
	return m_team_num;
}

void ALabPawn::SetTeamNum(uint8 NewTeamNum)
{
	m_team_num = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}

uint32 ALabPawn::GetHealth()
{
	return m_health;
}

float ALabPawn::GetHealthPercentage()
{
	return ((float)m_health / GetMaxHealth());
}

uint32 ALabPawn::GetMaxHealth()
{
	return m_base_health;
}

uint32 ALabPawn::GetAttackRange() const
{
	return m_base_attack_range;
}

void ALabPawn::Client_PlayMeleeAnim()
{

	if ((m_health > 0.f) && MeleeAnim)
	{
		
		float duration = PlayAnimMontage(MeleeAnim);

	}
}

uint32 ALabPawn::GetSightDistance()
{
	return m_base_sight_distance;
}
/*
void ALabPawn::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	ILabTeamInterface* team_interface = InterfaceCast<ILabTeamInterface>(OtherActor);

	if (team_interface){
		ALabPawn* target = Cast<ALabPawn>(OtherActor);
		if (target)
		{
			ULabBlueprintLibrary::printDebugInfo("add a target:");
			//AllTargets.AddUnique(OtherActor);
		}

	}
}

void ALabPawn::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ILabTeamInterface* team_interface = InterfaceCast<ILabTeamInterface>(OtherActor);
	if (team_interface){
		ALabPawn* target = Cast<ALabPawn>(OtherActor);
		if (target)
		{
			ULabBlueprintLibrary::printDebugInfo("remove a target:");
			//AllTargets.Remove(OtherActor);
		}
	}
}
*/