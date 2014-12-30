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

	virtual uint32 GetHealth() const = 0;

	virtual float GetHealthPercentage() const = 0;

	virtual bool HasMana() const = 0;

	virtual uint32 getMana() const = 0;

	virtual float GetManaPercentage()const = 0;
};
