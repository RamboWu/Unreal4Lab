// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once


#include "LabTeamInterface.generated.h"

/** Interface for actors which can be associated with teams */
UINTERFACE()
class ULabTeamInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()


};

class ILabTeamInterface
{
	GENERATED_IINTERFACE_BODY()

	/** returns the team number of the actor */
	virtual uint8 GetTeamNum() const = 0;
};
