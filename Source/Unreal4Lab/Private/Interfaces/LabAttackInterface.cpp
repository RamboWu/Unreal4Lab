#include "Unreal4Lab.h"
#include "LabAttackInterface.h"

ULabAttackInterface::ULabAttackInterface(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

uint32 ILabAttackInterface::GetAttackPriority(AActor Attacker) const
{
	return 0;
}

AActor* ILabAttackInterface::GetEnemy() const
{
	return NULL;
}

bool ILabAttackInterface::IsValidToAttack() const
{
	return true;
}

float ILabAttackInterface::GetDamage() const
{
	return 0.f;
}
