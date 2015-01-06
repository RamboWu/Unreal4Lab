// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawnReplicationInfo.h"


ULabPawnReplicationInfo::ULabPawnReplicationInfo(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//bReplicates = true;
}

void ULabPawnReplicationInfo::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULabPawnReplicationInfo, HealthMax);
	DOREPLIFETIME(ULabPawnReplicationInfo, HealthRegenerationAmount);
}
