// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "Pawns/LabPawn.h"
#include "LabBlueprintLibrary.h"
#include "LabSpell.h"


ALabSpell::ALabSpell(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	bInCoolDown = false;
	Level = -1;
}

void ALabSpell::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALabSpell, OrderIndex);
	DOREPLIFETIME(ALabSpell, Level);
	DOREPLIFETIME(ALabSpell, PawnOwner);
}

bool ALabSpell::CanCast()
{
	//ALabPawn* Pawn;

	// Check if the spell even has an active ability or not
	if (!HasActive)
	{
		return false;
	}

	// Check if the spell has any levels
	if (Level < 0)
	{
		
		SendPlayerMessage(NSLOCTEXT("Lab", "HUD", "SpellNotAvailable"));
		return false;
	}

	// Check if the spell is cooling down
	if (bInCoolDown)
	{
		SendPlayerMessage(NSLOCTEXT("Lab", "HUD", "SpellNotReady"));
		return false;
	}

	// Check if the pawn has enough mana
	ALabPawn* Pawn = ::Cast<ALabPawn>(PawnOwner);
	if (Pawn && Pawn->getMana() < GetManaCost())
	{
		SendPlayerMessage(NSLOCTEXT("Lab", "HUD", "NotEnoughMana"));
		return false;
	}

	return true;
}

void ALabSpell::SendPlayerMessage(FText Msg)
{
	ULabBlueprintLibrary::printDebugInfo(Msg.ToString());
// 	local UDKMOBAHeroAIController UDKMOBAHeroAIController;
// 	local UDKMOBAHUD UDKMOBAHUD;
// 
// 	// Ensure we have a pawn
// 	if (PawnOwner == None)
// 	{
// 		return;
// 	}
// 
// 	// Ensure we have a hero AI controller
// 	UDKMOBAHeroAIController = UDKMOBAHeroAIController(PawnOwner.Controller);
// 	if (UDKMOBAHeroAIController == None || UDKMOBAHeroAIController.UDKMOBAPlayerController == None)
// 	{
// 		return;
// 	}
// 
// 	UDKMOBAHUD = UDKMOBAHUD(UDKMOBAHeroAIController.UDKMOBAPlayerController.MyHUD);
// 	if (UDKMOBAHUD != None && UDKMOBAHUD.HUDMovie != None)
// 	{
// 		UDKMOBAHUD.HUDMovie.AddCenteredMessage(Message);
// 	}
}

void ALabSpell::Cast()
{
	ALabPawn* Pawn;

	// Consume mana
	Pawn = ::Cast<ALabPawn>(PawnOwner);
	if (Pawn)
	{
		Pawn->Mana = FMath::Max(Pawn->Mana - GetManaCost(), 0.f);
		//Pawn.Mana = Max(UDKMOBAPawn.Mana - GetManaCost(), 0);
	}

	// Spawn in cast effects
	CastEffects(false);

	// Perform the actual cast
	PerformCast();
}

void ALabSpell::Activate()
{
	// Immediately perform the spell effects
	ActivateEffects(false);

	if ( FMath::Abs(GetActivationTime()) < 0.0001f)
	{
		// Immediately perform action
		Cast();
	}
	else
	{
		// Perform action after cast time
		GetWorldTimerManager().SetTimer(this, &ALabSpell::Cast, GetActivationTime(), false);
	}

	if (HasAuthority())
	{
		// Start the cooldown timer
		bInCoolDown = true;
		GetWorldTimerManager().SetTimer(this, &ALabSpell::CooldownTimer, GetCooldownTime(), false);
	}
}

bool ALabSpell::SetTargetsFromAim(FVector AimLocation, FVector AimDirection)
{
	/*local Actor Actor;
	local vector HitLocation, HitNormal;

	if (AimingType == EAT_World)
	{
		// Perform a trace to find either UDKMOBAGroundBlockingVolume or the WorldInfo [BSP] 
		ForEach TraceActors(class'Actor', Actor, HitLocation, HitNormal, AimLocation + AimDirection * 16384.f, AimLocation)
		{
			if (UDKMOBAGroundBlockingVolume(Actor) != None || WorldInfo(Actor) != None)
			{
				TargetLocation = HitLocation;
				break;
			}
		}

		return true;
	}*/

	return false;
}

float ALabSpell::AdjustDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	return DamageAmout;
}

void ALabSpell::PerformCast()
{
	
}

void ALabSpell::CooldownTimer()
{
	bInCoolDown = false;
}

float ALabSpell::GetManaCost()
{
	if (ManaCost.Num() == 0)
	{
		return 0.f;
	}

	if (Level < 0)
	{
		return 0.f;
	}

	if (ManaCost.Num() <= Level)
	{
		// undefined - return best estimate
		return ManaCost[ManaCost.Num() - 1];
	}

	return ManaCost[Level];
}

float ALabSpell::GetActivationTime()
{
	if (ActivationTime.Num() == 0)
	{
		return 0.f;
	}

	if (Level < 0)
	{
		return 0.f;
	}

	if (ActivationTime.Num() <= Level)
	{
		// undefined - return best estimate
		return ActivationTime[ActivationTime.Num() - 1];
	}

	return ActivationTime[Level];
}

float ALabSpell::GetCooldownTime()
{
	if (CooldownTime.Num() == 0)
	{
		return 1.f;
	}

	if (Level < 0)
	{
		return 60.f;
	}

	if (CooldownTime.Num() <= Level)
	{
		// undefined - return best estimate
		return CooldownTime[CooldownTime.Num() - 1];
	}

	return CooldownTime[Level];
}

void ALabSpell::ActivateEffects(bool ViaReplication)
{

}

void ALabSpell::CastEffects(bool ViaReplication)
{

}
