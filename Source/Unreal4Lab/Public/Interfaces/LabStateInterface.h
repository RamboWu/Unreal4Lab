#pragma once

#include "LabStateInterface.generated.h"

/** Interface for actors which can be associated with state */
UINTERFACE()
class ULabStateInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()


};

class ILabStateInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual uint32 GetHealth();

	virtual float GetHealthPercentage();

	virtual uint32 GetMaxHealth();

	virtual bool HasMana();

	virtual uint32 getMana();

	virtual float GetManaPercentage();

	virtual uint32 GetAttackRange() const;

	virtual uint32 GetSightDistance();
};
