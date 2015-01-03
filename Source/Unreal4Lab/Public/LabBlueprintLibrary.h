// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "LabBlueprintLibrary.generated.h"

UCLASS()
class ULabBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

		/**
		 * Are the two actors on the same team.
		 *
		 * @param Actor1	First actor to compare.
		 * @param Actor2	Second actor to compare.
		 *
		 * @returns true if the actors are on the same team.
		 */
		 //UFUNCTION(BlueprintPure, Category=Game)
		 //static bool AreFriends(AActor* Actor1, AActor* Actor2);
	static void printDebugInfo(FString msg);
};
