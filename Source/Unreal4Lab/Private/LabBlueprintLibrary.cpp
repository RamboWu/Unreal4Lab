// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Unreal4Lab.h"
#include "LabBlueprintLibrary.h"


ULabBlueprintLibrary::ULabBlueprintLibrary(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void ULabBlueprintLibrary::printDebugInfo(FString msg)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, msg);
}
